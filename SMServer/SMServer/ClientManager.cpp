#include "Stdafx.h"
#include "ClientSession.h"
#include "Synchronizer.h"

#include "ClientManager.h"

namespace SM
{
	ClientManager* g_ClientManager;

	ClientManager::ClientManager()
	{
		InitializeCriticalSection(&m_critical_section);
	}
	ClientManager::~ClientManager()
	{

	}

	ClientSession* ClientManager::CreateClient(SOCKET p_socket)
	{
		ClientSession* client = new ClientSession(p_socket);
		m_client_list.insert(ClientList::value_type(p_socket, client));

		return client;
	}

	void ClientManager::FlushClientSend()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		for (auto& it : m_client_list)
		{
			ClientSession* client = it.second;
			if (false == client->SendFlush())
			{
				client->Disconnect();
			}
		}
	}

	void ClientManager::BroadcastPacket(ClientSession* p_client, PacketHeader* p_packet)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		for (auto& it : m_client_list)
		{
			ClientSession* client = it.second;
			if (p_client == client)
			{
				continue;
			}

			client->SendRequest(p_packet);
		}
	}

	void ClientManager::OnPeriodWork()
	{
		/// 접속이 끊긴 세션들 주기적으로 정리 (1초 정도 마다 해주자)
		DWORD currTick = GetTickCount();
		if (currTick - m_last_gc_tick >= 1000)
		{
			CollectGarbageSessions();
			m_last_gc_tick = currTick;
		}

		/// 접속된 클라이언트 세션별로 주기적으로 해줘야 하는 일 (주기는 알아서 정하면 됨 - 지금은 1초로 ㅎㅎ)
		if (currTick - m_last_client_work_tick >= 1000)
		{
			ClientPeriodWork();
			m_last_client_work_tick = currTick;
		}
	}

	void ClientManager::CollectGarbageSessions()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		std::vector<ClientSession*> disconnectedSessions;

		for (auto& it : m_client_list)
		{
			ClientSession* client = it.second;

			if (false == client->IsConnected() && false == client->IsDoingOverlappedOperation())
				disconnectedSessions.push_back(client);
		}

		for (auto& it : disconnectedSessions)
		{
			m_client_list.erase(it->m_socket);
			delete it;
		}
	}

	void ClientManager::ClientPeriodWork()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		for (auto& it : m_client_list)
		{
			ClientSession* client = it.second;
			client->OnTick();
		}
	}
}