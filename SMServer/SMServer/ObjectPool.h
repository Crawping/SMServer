#pragma once

template <class TOBJECT, int ALLOC_COUNT = 100>
class ObjectPool
{
public:

	static void* operator new(size_t objSize)
	{
		if (!m_free_list)
		{
			m_free_list = new uint8_t[sizeof(TOBJECT)*ALLOC_COUNT];

			uint8_t* pNext = m_free_list;
			uint8_t** ppCurr = reinterpret_cast<uint8_t**>(m_free_list);

			for (int i = 0; i<ALLOC_COUNT - 1; ++i)
			{
				/// OBJECT�� ũ�Ⱑ �ݵ�� ������ ũ�⺸�� Ŀ�� �Ѵ�
				pNext += sizeof(TOBJECT);
				*ppCurr = pNext;
				ppCurr = reinterpret_cast<uint8_t**>(pNext);
			}

			*ppCurr = 0; ///< �������� 0���� ǥ��
			m_total_alloc_count += ALLOC_COUNT;
		}

		uint8_t* pAvailable = m_free_list;
		m_free_list = *reinterpret_cast<uint8_t**>(pAvailable);
		++m_current_use_count;

		return pAvailable;
	}

	static void	operator delete(void* obj)
	{
		assert(m_current_use_count > 0);

		--m_current_use_count;

		*reinterpret_cast<uint8_t**>(obj) = m_free_list;
		m_free_list = static_cast<uint8_t*>(obj);
	}


private:
	static uint8_t*	m_free_list;
	static int		m_total_alloc_count; ///< for tracing
	static int		m_current_use_count; ///< for tracing


};


template <class TOBJECT, int ALLOC_COUNT>
uint8_t* ObjectPool<TOBJECT, ALLOC_COUNT>::m_free_list = nullptr;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::m_total_alloc_count = 0;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::m_current_use_count = 0;