#pragma once

namespace SM
{
	// ���� ��ü, �̱������� ������ش�.
	class SMServer
	{
	private:
		static SMServer* m_instance;
	public:
		static SMServer* GetInstance();
		static void Release();
	private:
		SMServer();
		~SMServer();
	};
}