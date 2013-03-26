// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "CacheHelperModuleFactory.h"

HMODULE g_hCurrentInstance;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        g_hCurrentInstance = hModule;
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

HRESULT __stdcall RegisterModule(DWORD dwServerVersion, 
                                 IHttpModuleRegistrationInfo* pModuleInfo,
                                 IHttpServer* pGlobalInfo)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    return pModuleInfo->SetRequestNotifications(new CCacheHelperModuleFactory, RQ_SEND_RESPONSE, 0);
}