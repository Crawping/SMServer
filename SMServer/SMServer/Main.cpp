#include "Stdafx.h"

#include "SMServer.h"

using namespace SM;

int main(void)
{
	g_LogManager = new LogManager;
	g_SMServer = new SMServer;

	g_LogManager->RegisterFile(10, "log_test.txt");

	for (int i = 0; i < 10; i++)
		g_LogManager->Logging(10, "���Ͽ� �α׸� �����! [%d]\r\n", i);
	for (int i = 0; i < 10; i++)
		g_LogManager->Logging("�ֿܼ� �α׸� �����! [%d]\n", i);

	g_SMServer->Release();
	g_LogManager->Release();

	return 0;
}