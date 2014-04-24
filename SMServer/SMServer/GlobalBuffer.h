#pragma once

namespace SM
{
	// 글로벌 버퍼 크기 설정, 기본 100MB
	// 최대 2^64 bytes
	const static unsigned __int64 g_global_buffer_size = 100 * 1024 * 1024;
	// 세션에 떼어줄 버퍼 크기 설정, 기본 1KB
	const static unsigned __int64 g_session_buffer_size = 1 * 1024;
	// 기본 사이즈로, 세션 102400개에 할당 가능
	class GlobalBuffer
	{
	public:
		GlobalBuffer();
		~GlobalBuffer();

	public:
		char* GetFreeBufferPos();

	private:
		char* m_buffer;
	private:
		list<char*> m_free_buffer_list;
	};

	extern GlobalBuffer* g_GlobalBuffer;
}