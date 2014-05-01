#include "Stdafx.h"
#include "Utility.h"
#include "ClientSession.h"
#include "CircularBuffer.h"

#include "PacketHandler.h"

namespace SM
{
	HandlerFunction g_handler_function_table[PKT_MAX];

	static void DefaultHandler(ClientSession* p_client_session, PacketHeader& p_packet)
	{
		assert(false);
		p_client_session->Disconnect();
	}

	struct RegisterHandler
	{
		RegisterHandler(int p_packet_type, HandlerFunction p_handler_function)
		{
			g_handler_function_table[p_packet_type] = p_handler_function;
		}
	};

	void InitializeHandlerFunction()
	{
		for (int i = 0; i < PKT_MAX; ++i)
		{
			if (g_handler_function_table[i] == 0)
			{
				g_handler_function_table[i] = DefaultHandler;
			}
		}
	}

	// 여기에 패킷 핸들러를 추가합니다.
	// (ClientSession* p_client_session, PacketHeader& p_packet)
	PACKET_HANDLING(PKT_CS_LOGIN)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		printf("PKT_CS_LOGIN\n");

		LoginRequest in_packet = static_cast<LoginRequest&>(p_packet);
		p_client_session->m_recv_buffer->Read((char*)&in_packet, in_packet.m_size);

		printf("Client's ID : [%s]\n", in_packet.m_player_id);

		LoginResult outPacket;
		strcpy_s(p_client_session->m_player_id, in_packet.m_player_id);
		strcpy_s(outPacket.m_player_id, p_client_session->m_player_id);
		outPacket.m_pos_x = p_client_session->m_pos_x = 0;
		outPacket.m_pos_y = p_client_session->m_pos_y = 0;

		if (!p_client_session->Broadcast(&outPacket))
		{
			p_client_session->Disconnect();
		}
	}

	PACKET_HANDLING(PKT_CS_CHAT)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		//ChatBroadcastRequest inPacket = static_cast<ChatBroadcastRequest&>(p_packet);
		//p_client_session->m_recv_buffer->Read((char*)&inPacket, inPacket.m_size);

		//ChatBroadcastResult outPacket;
		//outPacket.mPlayerId = inPacket.mPlayerId;
		//sprintf_s(outPacket.mName, "player%d", inPacket.mPlayerId);
		//strcpy_s(outPacket.mChat, inPacket.mChat);

		//// printf("%s : %s\n", outPacket.mName, outPacket.mChat);

		///// 채팅은 바로 방송 하면 끝
		//if (!p_client_session->Broadcast(&outPacket))
		//{
		//	p_client_session->Disconnect();
		//}
	}
}