#include "Stdafx.h"

#include "Utility.h"

using namespace SM;

int main(void)
{
	LogManager::GetInstance()->RegisterFile(10, "log_test.txt");

	for (int i = 0; i < 10; i++)
		LogManager::GetInstance()->Logging(10, "���Ͽ� �α׸� �����! [%d]\r\n", i);
	for (int i = 0; i < 10; i++)
		LogManager::GetInstance()->Logging("�ֿܼ� �α׸� �����! [%d]\n", i);

	return 0;
}