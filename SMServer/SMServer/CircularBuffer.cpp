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

		/// A, B ���� �Ѵ� �����Ͱ� �ִ� ���� A���� �д´�
		if (m_A_region_size > 0)
		{
			aRead = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			memcpy(destbuf, m_A_region_pointer, aRead);
			cnt -= aRead;
		}

		/// �б� �䱸�� �����Ͱ� �� �ִٸ� B �������� �д´�
		if (cnt > 0 && m_B_region_size > 0)
		{
			assert(cnt <= m_B_region_size);

			/// ������ ���� �� �б�
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


		/// A, B ���� �Ѵ� �����Ͱ� �ִ� ���� A���� �д´�
		if (m_A_region_size > 0)
		{
			aRead = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			memcpy(destbuf, m_A_region_pointer, aRead);
			m_A_region_size -= aRead;
			m_A_region_pointer += aRead;
			cnt -= aRead;
		}

		/// �б� �䱸�� �����Ͱ� �� �ִٸ� B �������� �д´�
		if (cnt > 0 && m_B_region_size > 0)
		{
			assert(cnt <= m_B_region_size);

			/// ������ ���� �� �б�
			size_t bRead = cnt;

			memcpy(destbuf + aRead, m_B_region_pointer, bRead);
			m_B_region_size -= bRead;
			m_B_region_pointer += bRead;
			cnt -= bRead;
		}

		assert(cnt == 0);

		/// A ���۰� ����ٸ� B���۸� �� ������ ���� A ���۷� ���� 
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
				/// B�� �ƹ��͵� ���� ��� �׳� A�� ����ġ
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

		/// Read�� �ݴ�� B�� �ִٸ� B������ ���� ����
		if (m_B_region_pointer != nullptr)
		{
			if (GetBFreeSpace() < bytes)
				return false;

			memcpy(m_B_region_pointer + m_B_region_size, data, bytes);
			m_B_region_size += bytes;

			return true;
		}

		/// A�������� �ٸ� ������ �뷮�� �� Ŭ ��� �� ������ B�� �����ϰ� ���
		if (GetAFreeSpace() < GetSpaceBeforeA())
		{
			AllocateB();

			if (GetBFreeSpace() < bytes)
				return false;

			memcpy(m_B_region_pointer + m_B_region_size, data, bytes);
			m_B_region_size += bytes;

			return true;
		}
		/// A������ �� ũ�� �翬�� A�� ����
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

		/// Read�� ���������� A�� �ִٸ� A�������� ���� ����

		if (m_A_region_size > 0)
		{
			size_t aRemove = (cnt > m_A_region_size) ? m_A_region_size : cnt;
			m_A_region_size -= aRemove;
			m_A_region_pointer += aRemove;
			cnt -= aRemove;
		}

		// ������ �뷮�� �� ������� B���� ���� 
		if (cnt > 0 && m_B_region_size > 0)
		{
			size_t bRemove = (cnt > m_B_region_size) ? m_B_region_size : cnt;
			m_B_region_size -= bRemove;
			m_B_region_pointer += bRemove;
			cnt -= bRemove;
		}

		/// A������ ������� B�� A�� ����ġ 
		if (m_A_region_size == 0)
		{
			if (m_B_region_size > 0)
			{
				/// ������ ��� ���̱�
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