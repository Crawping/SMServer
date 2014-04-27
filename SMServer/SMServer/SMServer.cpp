#include "Stdafx.h"
#include "Utility.h"
#include "ProducerConsumerQueue.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include "GlobalBuffer.h"
#include "PacketHandler.h"

#include "SMServer.h"

CRITICAL_SECTION g_critical_section;

namespace SM
{
	SMServer* g_SMServer = nullptr;
	ProducerConsumerQueue<SOCKET, accept_queue_limit> g_accept_socket_queue;

	SMServer::SMServer()
		: m_listener(0), m_server_iocp_handle(0), m_accept_thread_handle(0)
	{
		InitializeCriticalSectionAndSpinCount(&g_critical_section, 0x80000400);
		InitializeHandlerFunction();

		g_LogManager = new LogManager;
		g_GlobalBuffer = new GlobalBuffer;
		g_ClientManager = new ClientManager;
	}
	SMServer::~SMServer()
	{

	}
	void SMServer::Release()
	{
		for_each(m_server_worker_thread_handle_vector.begin(), m_server_worker_thread_handle_vector.end(),
			[&](vector<HANDLE>::const_reference it)
			{
				CloseHandle(it);
			}
		);

		g_LogManager->Release();
		g_GlobalBuffer->Release();

		delete g_LogManager;
		delete g_GlobalBuffer;
		delete g_ClientManager;
	}

	bool SMServer::Initializing(unsigned short p_port)
	{
		WORD version;
		WSADATA wsadata;
		SOCKADDR_IN server_adder;

		// Winsock
		version = MAKEWORD(2, 2);
		if (WSAStartup(version, &wsadata) != NULL)
		{
			g_LogManager->Logging("Error : WSAStartup() : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// Open socket
		m_listener = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_listener == INVALID_SOCKET)
		{
			g_LogManager->Logging("Fail to socket() : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// nagle algorithm
		BOOL opt = TRUE;
		setsockopt(m_listener, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

		// bind
		ZeroMemory(&server_adder, sizeof(server_adder));
		server_adder.sin_family = AF_INET;
		server_adder.sin_addr.s_addr = htonl(INADDR_ANY);
		server_adder.sin_port = htons(p_port);
		if (bind(m_listener, (LPSOCKADDR)&server_adder, sizeof(server_adder)) == SOCKET_ERROR)
		{
			g_LogManager->Logging("Fail to bind() : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// listen
		if (listen(m_listener, accept_queue_limit) == SOCKET_ERROR)
		{
			g_LogManager->Logging("Fail to listen() : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// iocp 핸들 생성
		m_server_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (INVALID_HANDLE_VALUE == m_server_iocp_handle)
		{
			g_LogManager->Logging("Fail to CreateIoCompletionPort : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		g_LogManager->Logging("Success to Initializing()\n");

		return true;
	}

	bool SMServer::Run()
	{
		// 워커 쓰레드 생성
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		DWORD thread_id;
		HANDLE thread_handle;
		for (int i = 0; i < (int)system_info.dwNumberOfProcessors * 2; ++i)
		//for (int i = 0; i < (int)1; ++i)
		{
			thread_handle = (HANDLE)_beginthreadex(NULL, 0, WorkerThreadFunction, NULL, 0, (unsigned int*)&thread_id);

			if (thread_handle == NULL)
			{
				g_LogManager->Logging("Fail to create worker threads.\n");
				assert(false);

				return false;
			}
			m_server_worker_thread_handle_vector.push_back(thread_handle);
		}

		SOCKET accepted_socket;
		while (true)
		{
			accepted_socket = accept(m_listener, NULL, NULL);
			if (accepted_socket == INVALID_SOCKET)
			{
				g_LogManager->Logging("Error : Accept Loop : Invalid Socket\n");
				continue;
			}

			// User -> Kernel -> User으로 가기 때문에 느리다. 나중에 바꿔주자.
			// QueueUserAPC(ProcessAcceptAPCFunction, m_accept_thread_handle, (ULONG)accepted_socket);
			g_accept_socket_queue.Produce(accepted_socket);
		}

		return true;
	}

	// 워커 쓰레드
	// 서버 메인 로직 처리 부분
	unsigned int WINAPI SMServer::WorkerThreadFunction(LPVOID p_param)
	{
		SOCKET accepted_socket;

		// 타이머 쓰레드 생성
		HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
		if (hTimer == NULL)
		{
			return false;
		}

		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -10000000; ///< 1초 후부터 동작
		if (!SetWaitableTimer(hTimer, &liDueTime, 100, TimerThreadFunction, NULL, TRUE))
		{
			return false;
		}

		while (true)
		{
			// Create ClientSession and Register to ClientSession Map
			if (g_accept_socket_queue.Consume(accepted_socket, false))
			{
				SYNCHRONIZE_CS(&g_critical_section);

				ClientSession* client_session = g_ClientManager->CreateClient(accepted_socket);

				SOCKADDR_IN sockaddr_in;
				int sockaddr_in_len = sizeof(sockaddr_in);
				getpeername(accepted_socket, (SOCKADDR*)&sockaddr_in, &sockaddr_in_len);

				if (false == client_session->OnConnect(&sockaddr_in))
				{
					g_LogManager->Logging("Fail connected\n");
					client_session->Disconnect();
				}

				continue;
			}

			// Flush Send
			g_ClientManager->FlushClientSend();

			//printf("Sleep Start\n");
			SleepEx(INFINITE, TRUE);
			//printf("Sleep End\n");
		}

		CloseHandle(hTimer);
	}

	// 타이머 쓰레드
	// 주기적으로 해줘야 할 부분
	void CALLBACK SMServer::TimerThreadFunction(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
	{
		g_ClientManager->OnPeriodWork();
	}
}