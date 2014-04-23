#pragma once

namespace SM
{
	// 서버 객체, 싱글톤으로 만들어준다.
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