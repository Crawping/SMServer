#include "Stdafx.h"

#include "Synchronizer.h"

namespace SM
{
	Synchronizer::Synchronizer(CRITICAL_SECTION* p_critical_section)
	{
		m_critical_section = p_critical_section;
		EnterCriticalSection(m_critical_section);
	}
	Synchronizer::~Synchronizer()
	{
		LeaveCriticalSection(m_critical_section);
	}
}