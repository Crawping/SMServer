#pragma once

#include "PacketDefine.h"

//static void Handler_##PKT_TYPE(ClientSession* p_client_session, PacketHeader& p_packet); \
//	static RegisterHandler _register_##PKT_TYPE(PKT_TYPE, Handler_##PKT_TYPE); \
//	static void Handler_##PKT_TYPE(ClientSession* p_client_session, PacketHeader& p_packet)
#define PACKET_HANDLING(PKT_TYPE)	\
	class Handler_##PKT_TYPE \
	{ \
	public: \
		static void Function_##PKT_TYPE(ClientSession* p_client_session, PacketHeader& p_packet); \
		friend class ClientSession; \
	}; \
	static RegisterHandler _register_##PKT_TYPE(PKT_TYPE, Handler_##PKT_TYPE::Function_##PKT_TYPE); \
	void Handler_##PKT_TYPE::Function_##PKT_TYPE(ClientSession* p_client_session, PacketHeader& p_packet)

namespace SM
{
	class ClientSession;
	struct PacketHeader;

	typedef void(*HandlerFunction)(ClientSession* p_client_session, PacketHeader& p_packet);

	extern HandlerFunction g_handler_function_table[PKT_MAX];

	void InitializeHandlerFunction();
}