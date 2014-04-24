#include "Stdafx.h"

#include "SMServer.h"

namespace SM
{
	SMServer* g_SMServer = nullptr;

	SMServer::SMServer()
		: m_listener(0), m_server_iocp_handle(0)
	{

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
	}

	bool SMServer::Initializing(unsigned short p_port)
	{
		WORD version;
		WSADATA wsadata;
		SOCKADDR_IN server_adder;

		// Winsock initialize
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

		// Turn off nagle algorithm
		BOOL opt = TRUE;
		setsockopt(m_listener, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

		// bind to socket
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

		// Start to listen
		if (listen(m_listener, accept_queue_limit) == SOCKET_ERROR)
		{
			g_LogManager->Logging("Fail to listen() : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// Create iocp handle
		m_server_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (INVALID_HANDLE_VALUE == m_server_iocp_handle)
		{
			g_LogManager->Logging("Fail to CreateIoCompletionPort : %d\n", WSAGetLastError());
			assert(false);

			return false;
		}

		// Create worker thread
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		DWORD thread_id;
		HANDLE thread_handle;
		for (int i = 0; i < (int)system_info.dwNumberOfProcessors * 2; ++i)
		{
			thread_handle = (HANDLE)_beginthreadex(NULL, 0, ProcessAcceptThreadFunction, NULL, 0, (unsigned int*)&thread_id);

			if (thread_handle == NULL)
			{
				g_LogManager->Logging("Fail to create worker threads.\n");
				assert(false);

				return false;
			}
			m_server_worker_thread_handle_vector.push_back(thread_handle);
		}

		g_LogManager->Logging("Success to Initializing()\n");


		return true;
	}

	bool SMServer::Run()
	{
		// Start accept thread
		DWORD thread_id;
		m_accept_thread_handle = (HANDLE)_beginthreadex(NULL, 0, ProcessAcceptThreadFunction, NULL, 0, (unsigned int*)&thread_id);
		if (m_accept_thread_handle == NULL)
		{
			g_LogManager->Logging("Fail to create accept thread.\n");
			assert(false);

			return false;
		}

		SOCKET accepted_socket;
		while (true)
		{
			accepted_socket = accept(m_listener, NULL, NULL);
			if (accepted_socket == INVALID_SOCKET)
			{
				g_LogManager->Logging("Error : Accept Loop : Invalid Socket\n");
			}

			QueueUserAPC(ProcessAcceptAPCFunction, m_accept_thread_handle, (ULONG)accepted_socket);
		}

		return true;
	}

	// 큐에 담긴 Accept 처리를 담당한다.
	unsigned int WINAPI SMServer::ProcessAcceptThreadFunction(LPVOID p_param)
	{
		while (true)
		{
			SleepEx(INFINITE, TRUE);
		}
	}
	void CALLBACK SMServer::ProcessAcceptAPCFunction(ULONG_PTR p_param)
	{
		SOCKET accepted_socket = (SOCKET)p_param;
		// Create ClientSession and Register to ClientSession Map
	}
}