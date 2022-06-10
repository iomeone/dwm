// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "init.h"

HMODULE g_hModule = NULL;
Dwm_Dram* pDwm = NULL;



DWORD WINAPI dwmWork(LPVOID lpThreadParameter)
{
	pDwm = new Dwm_Dram();

	BOOL done = FALSE;

	if (!pDwm->InitDwm())
	{
		return 0;
	}

	while (!done)
	{

		if (GetAsyncKeyState(VK_END) & 1) {
			done = TRUE;
		}
		if (done) {
			break;
		}

		render();

	}

	delete pDwm;
	pDwm = NULL;
	FreeLibraryAndExitThread(g_hModule, 0);
	

	return 0;
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        CloseHandle(CreateThread(NULL, 0, dwmWork, NULL, 0, 0));
         break;
    case DLL_THREAD_ATTACH:
         break;
    case DLL_THREAD_DETACH:
         break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

