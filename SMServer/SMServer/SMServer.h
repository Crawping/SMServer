#pragma once

#include "Utility.h"

namespace SM
{
	const static unsigned int accept_queue_limit = 100;

	// 서버 객체
	class SMServer
	{
	public:
		SMServer();
		~SMServer();
	public:
		void Release();

	public:
		// 서버 초기화
		bool Initializing(unsigned short p_port);

	public:
		// 서버 실행
		bool Run();

	private:
		// 큐에 담긴 Accept 처리를 담당한다.
		static unsigned int WINAPI ProcessAcceptThreadFunction(LPVOID p_param);
		static void CALLBACK ProcessAcceptAPCFunction(ULONG_PTR p_param);
		// 서버 메인 로직 처리 부분

	private:
		SOCKET m_listener;
		HANDLE m_server_iocp_handle;
	private:
		HANDLE m_accept_thread_handle;
	private:
		vector<HANDLE> m_server_worker_thread_handle_vector;
	};

	extern SMServer* g_SMServer;
}