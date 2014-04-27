#pragma once

namespace SM
{
	class ClientSession;
	struct PacketHeader;

	class ClientManager
	{
	public:
		ClientManager();
		~ClientManager();

	public:
		ClientSession* CreateClient(SOCKET p_socket);

	public:
		void FlushClientSend();

	public:
		void BroadcastPacket(ClientSession* p_client, PacketHeader* p_packet);

	public:
		void OnPeriodWork();
		void CollectGarbageSessions();
		void ClientPeriodWork();

	private:
		typedef map<SOCKET, ClientSession*> ClientList;
		ClientList m_client_list;
	private:
		bool m_flush_client_send;
		bool m_collect_garbage_sessions;
	private:
		DWORD m_last_gc_tick;
		DWORD m_last_client_work_tick;
	private:
		CRITICAL_SECTION m_critical_section;
	};

	extern ClientManager* g_ClientManager;
}