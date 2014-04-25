#pragma once

namespace SM
{
	class ClientSession
	{
	public:
		ClientSession(SOCKET p_socekt);
		~ClientSession();

	private:
		SOCKET m_socket;
	private:
		bool m_connected;
		bool m_logon;
	private:
		
	};
}