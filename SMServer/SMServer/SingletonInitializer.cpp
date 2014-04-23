#include "Stdafx.h"

#include "SingletonInitializer.h"

#include "LogManager.h"

namespace SM
{
	void SingletonInitializer()
	{
		g_LogManager = LogManager::GetInstance();
	}
}