#pragma once

namespace SM
{
	const static unsigned int accept_queue_limit = 100;

	// ���� ��ü
	class SMServer
	{
	public:
		SMServer();
		~SMServer();
	public:
		void Release();

	public:
		// ���� �ʱ�ȭ
		bool Initializing(unsigned short p_port);

	public:
		// ���� ����
		bool Run();

	private:
		// ��Ŀ ������
		// ���� ���� ���� ó�� �κ�
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