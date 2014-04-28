#include "Stdafx.h"
#include "Utility.h"
#include "GlobalBuffer.h"
#include "Synchronizer.h"

#include "CircularBuffer.h"

namespace SM
{
	bool CircularBuffer::Peek(OUT char* destbuf, size_t bytes)
	{
		SYNCHRONIZE_CS(&m_critical_section);

		assert(m_buffer != nullptr);

		if (m_A_region_size + m_B_region_size < bytes)
			return false;

		size_t cnt = bytes;
		size_t aRead = 0;

		/// A, B 영역 둘다 데이터가 있는 경우는 A먼저 읽는다
		if (m_A_region_size > 0)
		{
			aRead = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			memcpy(destbuf, m_A_region_pointer, aRead);
			cnt -= aRead;
		}

		/// 읽기 요구한 데이터가 더 있다면 B 영역에서 읽는다
		if (cnt > 0 && m_B_region_size > 0)
		{
			assert(cnt <= m_B_region_size);

			/// 남은거 마저 다 읽기
			size_t bRead = cnt;

			memcpy(destbuf + aRead, m_B_region_pointer, bRead);
			cnt -= bRead;
		}

		assert(cnt == 0);

		return true;

	}

	bool CircularBuffer::Read(OUT char* destbuf, size_t bytes)
	{
		SYNCHRONIZE_CS(&m_critical_section);

		assert(m_buffer != nullptr);

		if (m_A_region_size + m_B_region_size < bytes)
			return false;

		size_t cnt = bytes;
		size_t aRead = 0;


		/// A, B 영역 둘다 데이터가 있는 경우는 A먼저 읽는다
		if (m_A_region_size > 0)
		{
			aRead = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			memcpy(destbuf, m_A_region_pointer, aRead);
			m_A_region_size -= aRead;
			m_A_region_pointer += aRead;
			cnt -= aRead;
		}

		/// 읽기 요구한 데이터가 더 있다면 B 영역에서 읽는다
		if (cnt > 0 && m_B_region_size > 0)
		{
			assert(cnt <= m_B_region_size);

			/// 남은거 마저 다 읽기
			size_t bRead = cnt;

			memcpy(destbuf + aRead, m_B_region_pointer, bRead);
			m_B_region_size -= bRead;
			m_B_region_pointer += bRead;
			cnt -= bRead;
		}

		assert(cnt == 0);

		/// A 버퍼가 비었다면 B버퍼를 맨 앞으로 당기고 A 버퍼로 지정 
		if (m_A_region_size == 0)
		{
			if (m_B_region_size > 0)
			{
				if (m_B_region_pointer != m_buffer)
					memmove(m_buffer, m_B_region_pointer, m_B_region_size);

				m_A_region_pointer = m_buffer;
				m_A_region_size = m_B_region_size;
				m_B_region_pointer = nullptr;
				m_B_region_size = 0;
			}
			else
			{
				/// B에 아무것도 없는 경우 그냥 A로 스위치
				m_B_region_pointer = nullptr;
				m_B_region_size = 0;
				m_A_region_pointer = m_buffer;
				m_A_region_size = 0;
			}
		}

		return true;
	}




	bool CircularBuffer::Write(const char* data, size_t bytes)
	{
		SYNCHRONIZE_CS(&m_critical_section);

		assert(m_buffer != nullptr);

		/// Read와 반대로 B가 있다면 B영역에 먼저 쓴다
		if (m_B_region_pointer != nullptr)
		{
			if (GetBFreeSpace() < bytes)
				return false;

			memcpy(m_B_region_pointer + m_B_region_size, data, bytes);
			m_B_region_size += bytes;

			return true;
		}

		/// A영역보다 다른 영역의 용량이 더 클 경우 그 영역을 B로 설정하고 기록
		if (GetAFreeSpace() < GetSpaceBeforeA())
		{
			AllocateB();

			if (GetBFreeSpace() < bytes)
				return false;

			memcpy(m_B_region_pointer + m_B_region_size, data, bytes);
			m_B_region_size += bytes;

			return true;
		}
		/// A영역이 더 크면 당연히 A에 쓰기
		else
		{
			if (GetAFreeSpace() < bytes)
				return false;

			memcpy(m_A_region_pointer + m_A_region_size, data, bytes);
			m_A_region_size += bytes;

			return true;
		}
	}



	void CircularBuffer::Remove(size_t len)
	{
		SYNCHRONIZE_CS(&m_critical_section);

		size_t cnt = len;

		/// Read와 마찬가지로 A가 있다면 A영역에서 먼저 삭제

		if (m_A_region_size > 0)
		{
			size_t aRemove = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			m_A_region_size -= aRemove;
			m_A_region_pointer += aRemove;
			cnt -= aRemove;
		}

		// 제거할 용량이 더 남은경우 B에서 제거 
		if (cnt > 0 && m_B_region_size > 0)
		{
			size_t bRemove = (cnt > m_B_region_size) ? m_B_region_size : cnt;
			m_B_region_size -= bRemove;
			m_B_region_pointer += bRemove;
			cnt -= bRemove;
		}

		/// A영역이 비워지면 B를 A로 스위치 
		if (m_A_region_size == 0)
		{
			if (m_B_region_size > 0)
			{
				/// 앞으로 당겨 붙이기
				if (m_B_region_pointer != m_buffer)
					memmove(m_buffer, m_B_region_pointer, m_B_region_size);

				m_A_region_pointer = m_buffer;
				m_A_region_size = m_B_region_size;
				m_B_region_pointer = nullptr;
				m_B_region_size = 0;
			}
			else
			{
				m_B_region_pointer = nullptr;
				m_B_region_size = 0;
				m_A_region_pointer = m_buffer;
				m_A_region_size = 0;
			}
		}
	}
}