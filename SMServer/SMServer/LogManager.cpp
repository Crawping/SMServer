#include "Stdafx.h"

#include "LogManager.h"

namespace SM
{
	LogManager* g_LogManager = nullptr;

	LogManager::LogManager()
	{
		// 큐에 등록된 로그를 뽑아서 파일에 기록할 쓰레드를 하나 만들어 준다.
		m_thread_handle = (HANDLE)_beginthreadex(NULL, 0, LoggingThreadFunction, NULL, 0, (unsigned int*)&m_thread_id);
		if (m_thread_handle == NULL)
		{
			Logging("Fail to _beginthreadex, Thread HANDLE is NULL\n");
			assert(false);
		}

		fopen_s(&m_handle_map[LOG_DEFAULT], "default_log.txt", "a");
		if (m_handle_map[LOG_DEFAULT] == NULL)
		{
			Logging("Fail to register default logged file, FILE is NULL\n");
			assert(false);
		}
	}
	LogManager::~LogManager()
	{
		for_each(m_handle_map.begin(), m_handle_map.end(), 
			[&](map<unsigned int, FILE*>::const_reference it)
			{
				fclose(it.second);
			}
		);
	}
	void LogManager::Release()
	{
		if (m_thread_handle)
		{
			CloseHandle(m_thread_handle);
		}
	}

	// 로그 남길 파일을 생성하고 열어둔다.
	bool LogManager::RegisterFile(const int p_id, const char* p_filepath)
	{
		if (m_handle_map.find(p_id) != m_handle_map.end())
		{
			Logging("Fail to register logged file, id is already exist.\n");
			assert(false);

			return false;
		}

		fopen_s(&m_handle_map[p_id], p_filepath, "a");
		if (m_handle_map[p_id] == NULL)
		{
			m_handle_map.erase(m_handle_map.find(p_id));

			Logging("Fail to register logged file, FILE is NULL.\n");
			assert(false);

			return false;
		}
		
		return true;
	}

	// 로그를 콘솔에 바로 출력한다.
	void LogManager::Logging(const char* p_string, ...) const
	{
		va_list argument_list;

		va_start(argument_list, p_string);
		char* writeData = NULL;
		vasprintf(&writeData, p_string, argument_list);
		fprintf(stderr, writeData);
		va_end(argument_list);
	}
	// 로그를 큐에 등록한다.
	void LogManager::Logging(const int p_id, const char* p_string, ...) const
	{
		if (m_handle_map.find(p_id) == m_handle_map.end())
		{
			Logging("Can't find file of [%d] id.\n", p_id);
			assert(false);
		}

		va_list argument_list;

		va_start(argument_list, p_string);

		char* write_data = NULL;
		int length = vasprintf(&write_data, p_string, argument_list);

		LogData* log_data = new LogData;
		log_data->m_id = p_id;
		log_data->m_write_data = write_data;
		QueueUserAPC(LoggingAPCFunction, m_thread_handle, (ULONG_PTR)log_data);

		va_end(argument_list);
	}

	// 등록된 큐를 뽑아서 기록한다.
	unsigned int WINAPI LogManager::LoggingThreadFunction(LPVOID p_param)
	{
		while (true)
		{
			SleepEx(INFINITE, TRUE);
		}
	}
	void CALLBACK LogManager::LoggingAPCFunction(ULONG_PTR p_param)
	{
		LogData* log_data = (LogData*)p_param;
		fprintf_s(g_LogManager->m_handle_map[log_data->m_id], log_data->m_write_data);
		free(log_data->m_write_data);
	}
}