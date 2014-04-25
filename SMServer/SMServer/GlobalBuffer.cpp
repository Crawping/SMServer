#include "Stdafx.h"
#include "Utility.h"

#include "GlobalBuffer.h"

namespace SM
{
	GlobalBuffer* g_GlobalBuffer = nullptr;

	list<char*> GlobalBuffer::m_free_buffer_list;
	GlobalBuffer::GlobalBuffer()
	{
		ExtendBuffer();
	}
	GlobalBuffer::~GlobalBuffer()
	{

	}
	void GlobalBuffer::Release()
	{
		for_each(m_buffer.begin(), m_buffer.end(),
			[&](map<char*, char*>::reference it)
			{
				delete[] it.second;
			}
		);
	}

	void GlobalBuffer::ExtendBuffer()
	{
		char* buffer = new char[g_global_buffer_size];
		m_buffer[buffer] = buffer;
		char* pos = buffer;
		while (pos != buffer + g_global_buffer_size)
		{
			m_free_buffer_list.push_back(pos);
			pos += g_session_buffer_size;
		}
	}

	char* GlobalBuffer::PopFreeBufferPos()
	{
		SYNCHRONIZE_CS(&m_critical_section);

		if (m_free_buffer_list.empty())
		{
			g_LogManager->Logging("There is not free buffer list element. so it is extended.\n");
			ExtendBuffer();
		}

		char* free_buffer_pos = m_free_buffer_list.front();
		m_free_buffer_list.pop_front();
		return free_buffer_pos;
	}
	void GlobalBuffer::PushFreeBufferPos(char* p_free_buffer_pos)
	{
		SYNCHRONIZE_CS(&m_critical_section);

		m_free_buffer_list.push_back(p_free_buffer_pos);
	}
}