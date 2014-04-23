#include "Stdafx.h"

#include "SMServer.h"

namespace SM
{
	SMServer* SMServer::m_instance = nullptr;
	SMServer* SMServer::GetInstance()
	{
		if (m_instance == NULL) m_instance = new SMServer;
		return m_instance;
	}
	void SMServer::Release()
	{
		delete m_instance;
	}

	SMServer::SMServer()
	{

	}
	SMServer::~SMServer()
	{

	}
}