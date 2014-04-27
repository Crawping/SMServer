#include "Stdafx.h"
#include "Utility.h"
#include "PacketHandler.h"
#include "GlobalBuffer.h"
#include "CircularBuffer.h"
#include "ClientManager.h"

#include "ClientSession.h"

namespace SM
{

	ClientSession::ClientSession(SOCKET p_socket)
		: m_socket(p_socket), m_connected(false), m_logon(false)
	{

	}
	ClientSession::~ClientSession()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		delete m_send_buffer;
		delete m_recv_buffer;
		delete m_overlapped_send;
		delete m_overlapped_recv;
	}

	bool ClientSession::OnConnect(SOCKADDR_IN* p_sockaddr_in)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		memcpy(&m_sockaddr_in, p_sockaddr_in, sizeof(SOCKADDR_IN));

		u_long argument_non_blocking = 1;
		ioctlsocket(m_socket, FIONBIO, &argument_non_blocking);

		int argument_nagle = 1;
		setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)argument_nagle, sizeof(int));

		g_LogManager->Logging("A Client Session is connected: IP[%s], PORT[%d]\n", inet_ntoa(m_sockaddr_in.sin_addr), ntohs(m_sockaddr_in.sin_port));

		m_connected = true;

		m_send_buffer = new CircularBuffer(g_GlobalBuffer->PopFreeBufferPos(), g_session_buffer_size);
		m_recv_buffer = new CircularBuffer(g_GlobalBuffer->PopFreeBufferPos(), g_session_buffer_size);
		m_overlapped_send = new OverlappedIO;
		m_overlapped_recv = new OverlappedIO;
		
		return RequestRecv();
	}
	bool ClientSession::RequestRecv()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		if (m_connected == false)
		{
			return false;
		}

		DWORD recv_bytes = 0;
		DWORD flags = 0;
		WSABUF wsa_buffer;
		wsa_buffer.len = (ULONG)m_recv_buffer->GetFreeSpaceSize();
		wsa_buffer.buf = (char*)m_recv_buffer->GetBuffer();

		memset(m_overlapped_recv, 0, sizeof(OverlappedIO));
		m_overlapped_recv->m_client_session = this;

		if (SOCKET_ERROR == WSARecv(m_socket, &wsa_buffer, 1, &recv_bytes, &flags, (LPWSAOVERLAPPED)m_overlapped_recv, RecvCompletion))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				return false;
			}
		}

		IncreaseOverlappedRequestedCount();

		return true;
	}

	void ClientSession::Disconnect()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		if (m_connected == false)
		{
			return;
		}

		g_LogManager->Logging("A Client Session is disconnected: IP[%s], PORT[%d]\n", inet_ntoa(m_sockaddr_in.sin_addr), ntohs(m_sockaddr_in.sin_port));

		LINGER linger_option;
		linger_option.l_onoff = 1;
		linger_option.l_linger = 1;

		if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&linger_option, sizeof(LINGER)))
		{
			g_LogManager->Logging("setsockopt's linger option error: [%d]\n", GetLastError());
			return;
		}

		closesocket(m_socket);
		m_connected = false;
	}

	void ClientSession::OnRead(size_t p_length)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		m_recv_buffer->Commit(p_length);

		PacketHeader packet_header;

		while (true)
		{
			if (false == m_recv_buffer->Peek((char*)&packet_header, sizeof(PacketHeader)))
			{
				return;
			}

			if (m_recv_buffer->GetStoredSize() < (size_t)packet_header.m_size)
			{
				return;
			}

			if (packet_header.m_type >= PKT_MAX || packet_header.m_type <= PKT_NONE)
			{
				Disconnect();
				return;
			}

			// Handling
			g_handler_function_table[packet_header.m_type](this, packet_header);
		}
	}
	void ClientSession::OnWrite(size_t p_length)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		m_send_buffer->Remove(p_length);
	}

	bool ClientSession::SendRequest(PacketHeader* p_packet)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		if (m_connected == false)
		{
			return false;
		}

		if (false == m_send_buffer->Write((char*)p_packet, p_packet->m_size))
		{
			Disconnect();
			return false;
		}

		return true;
	}

	bool ClientSession::Broadcast(PacketHeader* p_packet)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		if (!SendRequest(p_packet))
		{
			return false;
		}

		if (m_connected == false)
		{
			return false;
		}

		g_ClientManager->BroadcastPacket(this, p_packet);

		return true;
	}

	bool ClientSession::SendFlush()
	{
		SYNCHRONIZE_CS(&g_critical_section);

		if (m_connected == false)
		{
			return false;
		}

		if (m_send_buffer->GetContiguiousBytes() == 0)
		{
			return true;
		}

		DWORD send_bytes = 0;
		DWORD flags = 0;

		WSABUF wsa_buffer;
		wsa_buffer.len = (ULONG)m_send_buffer->GetContiguiousBytes();
		wsa_buffer.buf = (char*)m_send_buffer->GetBufferStart();

		memset(m_overlapped_send, 0, sizeof(OverlappedIO));
		m_overlapped_send->m_client_session = this;

		if (SOCKET_ERROR == WSASend(m_socket, &wsa_buffer, 1, &send_bytes, flags, (LPWSAOVERLAPPED)m_overlapped_send, SendCompletion))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				return false;
			}
		}

		IncreaseOverlappedRequestedCount();

		return true;
	}

	void ClientSession::OnTick()
	{

	}







	void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		ClientSession* from_client = ((OverlappedIO*)lpOverlapped)->m_client_session;

		from_client->DecreaseOverlappedRequestedCount();

		if (!from_client->IsConnected())
		{
			return;
		}

		/// 에러 발생시 해당 세션 종료
		if (dwError || cbTransferred == 0)
		{
			from_client->Disconnect();
			return;
		}

		/// 받은 데이터 처리
		from_client->OnRead(cbTransferred);

		/// 다시 받기
		if (false == from_client->RequestRecv())
		{
			from_client->Disconnect();
			return;
		}
	}


	void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
	{
		SYNCHRONIZE_CS(&g_critical_section);

		ClientSession* from_client = ((OverlappedIO*)lpOverlapped)->m_client_session;

		from_client->DecreaseOverlappedRequestedCount();

		if (!from_client->IsConnected())
		{
			return;
		}

		/// 에러 발생시 해당 세션 종료
		if (dwError || cbTransferred == 0)
		{
			from_client->Disconnect();
			return;
		}

		from_client->OnWrite(cbTransferred);
	}
}