#pragma once

#include "GlobalBuffer.h"
#include "ObjectPool.h"

namespace SM
{
	class CircularBuffer : public ObjectPool<CircularBuffer>
	{
	public:

		CircularBuffer(char* p_buffer, size_t p_size) : m_B_region_pointer(nullptr), m_A_region_size(0), m_B_region_size(0)
		{
			InitializeCriticalSectionAndSpinCount(&m_critical_section, 0x80000400);
			m_buffer = p_buffer;
			m_bufferEnd = p_buffer + p_size;
			m_A_region_pointer = m_buffer;
		}

		~CircularBuffer()
		{
			g_GlobalBuffer->PushFreeBufferPos(m_buffer);
		}

		bool Peek(OUT char* destbuf, size_t bytes);
		bool Read(OUT char* destbuf, size_t bytes);
		bool Write(const char* data, size_t bytes);


		/// ������ ù�κ� len��ŭ ������
		void Remove(size_t len);

		size_t GetFreeSpaceSize()
		{
			if (m_B_region_pointer != nullptr)
				return GetBFreeSpace();
			else
			{
				/// A ���ۺ��� �� ���� �����ϸ�, B ���۷� ����ġ
				if (GetAFreeSpace() < GetSpaceBeforeA())
				{
					AllocateB();
					return GetBFreeSpace();
				}
				else
					return GetAFreeSpace();
			}
		}

		size_t GetStoredSize() const
		{
			return m_A_region_size + m_B_region_size;
		}

		size_t GetContiguiousBytes() const
		{
			if (m_A_region_size > 0)
				return m_A_region_size;
			else
				return m_B_region_size;
		}

		/// ���Ⱑ ������ ��ġ (������ ���κ�) ��ȯ
		void* GetBuffer() const
		{
			if (m_B_region_pointer != nullptr)
				return m_B_region_pointer + m_B_region_size;
			else
				return m_A_region_pointer + m_A_region_size;
		}




		/// Ŀ��(aka. IncrementWritten)
		void Commit(size_t len)
		{
			if (m_B_region_pointer != nullptr)
				m_B_region_size += len;
			else
				m_A_region_size += len;
		}

		/// ������ ù�κ� ����
		void* GetBufferStart() const
		{
			if (m_A_region_size > 0)
				return m_A_region_pointer;
			else
				return m_B_region_pointer;
		}


	private:

		void AllocateB()
		{
			m_B_region_pointer = m_buffer;
		}

		size_t GetAFreeSpace() const
		{
			return (m_bufferEnd - m_A_region_pointer - m_A_region_size);
		}

		size_t GetSpaceBeforeA() const
		{
			return (m_A_region_pointer - m_buffer);
		}


		size_t GetBFreeSpace() const
		{
			if (m_B_region_pointer == nullptr)
				return 0;

			return (m_A_region_pointer - m_B_region_pointer - m_B_region_size);
		}

	private:
		CRITICAL_SECTION m_critical_section;

		char*	m_buffer;
		char*	m_bufferEnd;

		char*	m_A_region_pointer;
		size_t	m_A_region_size;

		char*	m_B_region_pointer;
		size_t	m_B_region_size;
	};
}