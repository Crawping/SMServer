#pragma once

#include "Synchronizer.h"

namespace SM
{
	// �۷ι� ���� ũ�� ����, �⺻ 100MB
	// �ִ� 2^64 bytes
	const unsigned __int64 g_global_buffer_size = 100 * 1024 * 1024;
	// ���ǿ� ������ ���� ũ�� ����, �⺻ 10KB (Send, Recv Buffer ���� 5KB)
	const unsigned __int64 g_session_buffer_size =  100 * 1024;
	// �⺻ �������, ���� 102400���� �Ҵ� ����
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