#include "Stdafx.h"
#include "Utility.h"

#include "GlobalBuffer.h"

namespace SM
{
	GlobalBuffer* g_GlobalBuffer = nullptr;

	GlobalBuffer::GlobalBuffer()
	{
		m_buffer = new char[g_global_buffer_size];
		char* pos = m_buffer;
		while (pos != m_buffer + g_global_buffer_size)
		{
			m_free_buffer_list.push_back(pos);
			pos += g_session_buffer_size;
		}
	}
	GlobalBuffer::~GlobalBuffer()
	{
		delete[] m_buffer;
	}

	char* GlobalBuffer::GetFreeBufferPos()
	{
		char* free_pos = m_free_buffer_list.front();
		m_free_buffer_list.pop_front();
		return free_pos;
	}
}