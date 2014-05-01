#pragma once

#include "PacketDefine.h"
#include "ObjectPool.h"

#define REGISTER_PACKET_HANDLING(PKT_TYPE) friend class Handler_##PKT_TYPE

namespace SM
{
	class CircularBuffer;
	struct PacketHeader;
	class ClientSession;
	class OverlappedIO : public ObjectPool<OverlappedIO>
	{
	public:
		OverlappedIO() : m_client_session(nullptr) { }
		~OverlappedIO() { }

	public:
		OVERLAPPED m_overlapped;
		ClientSession* m_client_session;
	};
	/*struct OverlappedIO : public OVERLAPPED
	{
	public:
		OverlappedIO() : m_client_session(nullptr) { }

	public:
		ClientSession* m_client_session;
	};*/


	class ClientSession : public ObjectPool<ClientSession>
	{
	public:
		ClientSession(SOCKET p_socket);
		~ClientSession();

	public:
		bool OnConnect(SOCKADDR_IN* p_sockaddr_in);
		bool RequestRecv();

	public:
		void Disconnect();

	public:
		void OnRead(size_t p_length);
		void OnWrite(size_t p_length);

	public:
		bool SendRequest(PacketHeader* p_packet);
		bool Broadcast(PacketHeader* p_packet);
		bool SendFlush();

	public:
		void OnTick();

	public:
		bool IsConnected() const { return m_connected; }
		void IncreaseOverlappedRequestedCount() { ++m_overlapped_requested_count; }
		void DecreaseOverlappedRequestedCount() { --m_overlapped_requested_count; }
		bool IsDoingOverlappedOperation() const { return m_overlapped_requested_count > 0; }

	private:
		// 여기에 각 Player의 정보를 넣어줍니다.
		char m_player_id[MAX_NAME_LEN];
		double m_pos_x;
		double m_pos_y;
	private:
		SOCKET m_socket;
		SOCKADDR_IN m_sockaddr_in;
	private:
		bool m_connected;
		bool m_logon;
	private:
		CircularBuffer* m_send_buffer;
		CircularBuffer* m_recv_buffer;
		OverlappedIO* m_overlapped_send;
		OverlappedIO* m_overlapped_recv;
		int m_overlapped_requested_count;

		friend class ClientManager;

		// 여기에서 패킷을 등록합니다.
		REGISTER_PACKET_HANDLING(PKT_CS_LOGIN);
		REGISTER_PACKET_HANDLING(PKT_CS_CHAT);
	};

	void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
}