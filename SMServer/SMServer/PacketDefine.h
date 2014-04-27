#pragma once

#define MAX_CHAT_LEN	51

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40

namespace SM
{
	enum PacketTypes
	{
		PKT_NONE = 0,

		PKT_CS_LOGIN = 1,
		PKT_SC_LOGIN = 2,

		PKT_CS_CHAT = 3,
		PKT_SC_CHAT = 4,

		PKT_MAX = 5
	};

#pragma pack(push, 1)

	struct PacketHeader
	{
		PacketHeader() : m_size(0), m_type(PKT_NONE) { }
		short m_size;
		short m_type;
	};

	struct LoginRequest : public PacketHeader
	{
		LoginRequest()
		{
			m_size = sizeof(LoginRequest);
			m_type = PKT_CS_LOGIN;
			m_player_id = -1;
		}

		int	m_player_id;
	};

	struct LoginResult : public PacketHeader
	{
		LoginResult()
		{
			m_size = sizeof(LoginResult);
			m_type = PKT_SC_LOGIN;
			m_player_id = -1;
			memset(m_name, 0, MAX_NAME_LEN);
		}

		int m_player_id;
		double m_pos_x;
		double m_pos_y;
		double m_pos_z;
		char m_name[MAX_NAME_LEN];
	};

	struct ChatBroadcastRequest : public PacketHeader
	{
		ChatBroadcastRequest()
		{
			m_size = sizeof(ChatBroadcastRequest);
			m_type = PKT_CS_CHAT;
			mPlayerId = -1;

			memset(mChat, 0, MAX_CHAT_LEN);
		}

		int	mPlayerId;
		char mChat[MAX_CHAT_LEN];
	};

	struct ChatBroadcastResult : public PacketHeader
	{
		ChatBroadcastResult()
		{
			m_size = sizeof(ChatBroadcastResult);
			m_type = PKT_SC_CHAT;
			mPlayerId = -1;

			memset(mName, 0, MAX_NAME_LEN);
			memset(mChat, 0, MAX_CHAT_LEN);
		}

		int	mPlayerId;
		char mName[MAX_NAME_LEN];
		char mChat[MAX_CHAT_LEN];
	};

#pragma pack(pop)
}