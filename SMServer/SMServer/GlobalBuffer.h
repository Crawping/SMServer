#pragma once

namespace SM
{
	// �۷ι� ���� ũ�� ����, �⺻ 100MB
	// �ִ� 2^64 bytes
	const static unsigned __int64 g_global_buffer_size = 100 * 1024 * 1024;
	// ���ǿ� ������ ���� ũ�� ����, �⺻ 1KB
	const static unsigned __int64 g_session_buffer_size = 1 * 1024;
	// �⺻ �������, ���� 102400���� �Ҵ� ����
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