#include "Stdafx.h"

#include "Utility.h"

using namespace SM;

int main(void)
{
	g_LogManager = new LogManager;

	g_LogManager->RegisterFile(10, "log_test.txt");

	for (int i = 0; i < 10; i++)
		g_LogManager->Logging(10, "���Ͽ� �α׸� �����! [%d]\r\n", i);
	for (int i = 0; i < 10; i++)
		g_LogManager->Logging("�ֿܼ� �α׸� �����! [%d]\n", i);

	return 0;
}