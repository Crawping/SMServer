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

		PKT_CS_MOVE = 3,
		PKT_SC_MOVE = 4,

		PKT_CS_CHAT = 5,
		PKT_SC_CHAT = 6,

		PKT_MAX = 7
	};

	enum Direction
	{
		D_UP = 1,
		D_DOWN = 2,
		D_LEFT = 4,
		D_RIGHT = 8,
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
			memset(m_player_id, 0, MAX_NAME_LEN);
		}

		char 	m_player_id[MAX_NAME_LEN];
	};

	struct LoginResult : public PacketHeader
	{
		LoginResult()
		{
			m_size = sizeof(LoginResult);
			m_type = PKT_SC_LOGIN;
			memset(m_player_id, 0, MAX_NAME_LEN);
			m_pos_x = 0;
			m_pos_y = 0;
		}

		char 	m_player_id[MAX_NAME_LEN];
		double m_pos_x;
		double m_pos_y;
	};

	struct MoveRequest : public PacketHeader
	{
		MoveRequest()
		{
			m_size = sizeof(MoveRequest);
			m_type = PKT_CS_MOVE;
			m_direction = 0;
		}

		unsigned char m_direction;
	};

	struct MoveResult : public PacketHeader
	{
		MoveResult()
		{
			m_size = sizeof(MoveResult);
			m_type = PKT_SC_MOVE;
			m_pos_x = 0;
			m_pos_y = 0;
			m_direction = 0;
		}

		double m_pos_x;
		double m_pos_y;
		unsigned char m_direction;
	};

	struct ChatBroadcastRequest : public PacketHeader
	{
		ChatBroadcastRequest()
		{
			m_size = sizeof(ChatBroadcastRequest);
			m_type = PKT_CS_CHAT;
			memset(m_player_id, 0, MAX_NAME_LEN);

			memset(mChat, 0, MAX_CHAT_LEN);
		}

		char 	m_player_id[MAX_NAME_LEN];
		char mChat[MAX_CHAT_LEN];
	};

	struct ChatBroadcastResult : public PacketHeader
	{
		ChatBroadcastResult()
		{
			m_size = sizeof(ChatBroadcastResult);
			m_type = PKT_SC_CHAT;
			memset(m_player_id, 0, MAX_NAME_LEN);

			memset(mChat, 0, MAX_CHAT_LEN);
		}

		char 	m_player_id[MAX_NAME_LEN];
		char mChat[MAX_CHAT_LEN];
	};

#pragma pack(pop)
}