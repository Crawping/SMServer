#pragma once

// For Socket
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// For Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

// For Thread
#include <Process.h>

// For Critical Section
#include <WinBase.h>

// For Console Debug
#include <assert.h>
#include <stdio.h>

// For String
#include <string>
#include <string.h>
#include "vasprintf.h"

// For Lambda
#include <algorithm>

// For STL
#include <vector>
#include <queue>
#include <list>
#include <map>
using namespace std;