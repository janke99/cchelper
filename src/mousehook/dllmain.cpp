// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "mousehook.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CMouseHook::m_hModule = hModule;
		return TRUE;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		return TRUE;
	case DLL_PROCESS_DETACH:
		if( CMouseHook::m_hWnd )
		{
			//CMouseHook::StopHook();
		}
		break;
	}
	return TRUE;
}

