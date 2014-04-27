#include "Stdafx.h"
#include "Utility.h"

#include "SMServer.h"
#include "GlobalBuffer.h"
#include "ClientManager.h"

using namespace SM;

int main(void)
{
	g_SMServer = new SMServer;

	g_LogManager->RegisterFile(10, "log_test.txt");

	for (int i = 0; i < 10; i++)
		g_LogManager->Logging(10, "���Ͽ� �α׸� �����! [%d]\r\n", i);
	for (int i = 0; i < 10; i++)
		g_LogManager->Logging("�ֿܼ� �α׸� �����! [%d]\n", i);

	g_SMServer->Initializing(9001);
	g_SMServer->Run();

	g_SMServer->Release();
	delete g_SMServer;

	return 0;
}