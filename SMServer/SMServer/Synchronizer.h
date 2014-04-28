#pragma once

#ifndef SYNCHRONIZE_CS
#define SYNCHRONIZE_CS(name) // Synchronizer synchronizer(name)
#endif

namespace SM
{
	class Synchronizer
	{
	public:
		Synchronizer(CRITICAL_SECTION* p_critical_section);
		~Synchronizer();

	private:
		CRITICAL_SECTION* m_critical_section;
	};
}