#pragma once

#include "Utility.h"

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
		// ť�� ��� Accept ó���� ����Ѵ�.
		static unsigned int WINAPI ProcessAcceptThreadFunction(LPVOID p_param);
		static void CALLBACK ProcessAcceptAPCFunction(ULONG_PTR p_param);
		// ���� ���� ���� ó�� �κ�

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