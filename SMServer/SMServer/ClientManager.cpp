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
		if (TryEnterCriticalSection(&g_critical_section) == false)
		{
			return;
		}
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
		if (TryEnterCriticalSection(&g_critical_section) == false)
		{
			return;
		}
		SYNCHRONIZE_CS(&g_critical_section);

		/// ������ ���� ���ǵ� �ֱ������� ���� (1�� ���� ���� ������)
		DWORD currTick = GetTickCount();
		if (currTick - m_last_gc_tick >= 1000)
		{
			CollectGarbageSessions();
			m_last_gc_tick = currTick;
		}

		/// ���ӵ� Ŭ���̾�Ʈ ���Ǻ��� �ֱ������� ����� �ϴ� �� (�ֱ�� �˾Ƽ� ���ϸ� �� - ������ 1�ʷ� ����)
		if (currTick - m_last_client_work_tick >= 1000)
		{
			ClientPeriodWork();
			m_last_client_work_tick = currTick;
		}
	}

	void ClientManager::CollectGarbageSessions()
	{
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
		for (auto& it : m_client_list)
		{
			ClientSession* client = it.second;
			client->OnTick();
		}
	}
}