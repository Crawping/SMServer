#pragma once

#include "Synchronizer.h"

namespace SM
{
	// 글로벌 버퍼 크기 설정, 기본 100MB
	// 최대 2^64 bytes
	const unsigned __int64 g_global_buffer_size = 100 * 1024 * 1024;
	// 세션에 떼어줄 버퍼 크기 설정, 기본 10KB (Send, Recv Buffer 각각 5KB)
	const unsigned __int64 g_session_buffer_size =  100 * 1024;
	// 기본 사이즈로, 세션 102400개에 할당 가능
	class GlobalBuffer
	{
	public:
		GlobalBuffer();
		~GlobalBuffer();
	public:
		void Release();

	public:
		void ExtendBuffer();
	public:
		char* PopFreeBufferPos();
		void PushFreeBufferPos(char* p_free_buffer_pos);

	private:
		CRITICAL_SECTION m_critical_section;
	private:
		map<char*, char*> m_buffer;
	private:
		static list<char*> m_free_buffer_list;
	};

	extern GlobalBuffer* g_GlobalBuffer;
}