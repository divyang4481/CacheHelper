// CacheHelperTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\CacheConfiguration.h"

HMODULE g_hCurrentInstance = NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	CCacheConfiguration config;
	config.Initialize(L"D:\\src\\_lab\\CacheHelperTests\\CacheHelper.ini");

	return 0;
}

