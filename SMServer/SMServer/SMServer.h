#pragma once

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
		// 워커 쓰레드
		// 서버 메인 로직 처리 부분
		static unsigned int WINAPI WorkerThreadFunction(LPVOID p_param);
		static void CALLBACK TimerThreadFunction(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue);

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