#pragma once

namespace SM
{
	typedef struct _LogData
	{
		int m_id;
		char* m_write_data;
	} LogData;

	// id에 지정된 FILE이 없는 경우
	const static int LOG_DEFAULT = 0;
	// Log를 찍을 때는 항상 LogManager를 통해 찍자.
	// LogManager는 서버에 하나만 있으면 되므로 싱글톤 패턴을 적용한다.
	class LogManager
	{
	private:
		static LogManager* m_instance;
	public:
		static LogManager* GetInstance();
		static void Release();
	private:
		LogManager();
		~LogManager();

	public:
		// 로그 남길 파일을 생성하고 열어둔다.
		bool RegisterFile(const int p_id, const char* p_filepath);

	public:
		// 로그를 콘솔에 바로 출력한다.
		void Logging(const char* p_string, ...);
		// 로그를 큐에 등록한다.
		void Logging(const int p_id, const char* p_string, ...);

	private:
		// 등록된 큐를 뽑아서 기록한다.
		static unsigned int WINAPI LoggingThreadFunction(LPVOID p_param);
		static void CALLBACK LoggingAPCFunction(ULONG_PTR p_param);

	private:
		DWORD m_thread_id;
		HANDLE m_thread_handle;
	private:
		map<int, FILE*> m_handle_map;
	};

	// 싱글톤 패턴에서 GetInstance()의 퍼포먼스 문제를 해결하기 위해 글로벌 인스턴스를 하나 만들어준다.
	extern LogManager* g_LogManager;
}