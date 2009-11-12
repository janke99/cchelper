// mousehook.cpp : Defines the exported functions for the DLL application.
//

#include <windows.h>
#include "mousehook.h"
#include "log.h"
#include <tchar.h>

#pragma data_seg("TWH.ShareData")
HWND	CMouseHook::m_hWnd	= NULL;
HMODULE CMouseHook::m_hModule = NULL;
HHOOK   CMouseHook::m_hHook	= NULL;
#pragma data_seg()
#pragma comment(linker, "/section:TWH.ShareData,rws")




void ErrorMsg()
{
	LPVOID lpMsgBuf;

	FormatMessage(

		FORMAT_MESSAGE_ALLOCATE_BUFFER |

		FORMAT_MESSAGE_FROM_SYSTEM |

		FORMAT_MESSAGE_IGNORE_INSERTS,

		NULL,

		GetLastError(),

		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), // 既定の言語

		(LPTSTR) &lpMsgBuf,

		0,

		NULL

	);

	// lpMsgBuf 内のすべての挿入シーケンスを処理する。

	// ...

	// 文字列を表示する。

	base::Log(9, "Error:%s", lpMsgBuf );
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);

	// バッファを解放する。

	LocalFree(lpMsgBuf);

}

LRESULT CALLBACK CMouseHook::GetMsgProc(int nCode,
    WPARAM wParam,
    LPARAM lParam
)
{
	LPMSG msg = (LPMSG)lParam;

	if(msg->hwnd == m_hWnd )
	{
		base::Log(9,"%x",m_hWnd);
		switch(msg->message )
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			base::Log(9,"%x clicked",m_hWnd);
			msg->hwnd = 0;
			break;
		}
	}

    if(nCode < 0)
	{	/* pass it on */
		 CallNextHookEx(m_hHook, nCode, wParam, lParam);
		 return 0;
	} /* pass it on */

	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
} // msghook


BOOL CMouseHook::StartHook(HWND   hWnd)
{

	base::Log(9,"StartHook");
	m_hHook = SetWindowsHookEx(WH_GETMESSAGE,
			(HOOKPROC)GetMsgProc,
			m_hModule,
			0);

	if( m_hHook )
	{
		m_hWnd = hWnd;
		base::Log(9,"StartHook success=%x, %x",m_hHook, hWnd);
		return TRUE;
	}
	else
	{
		ErrorMsg();
		return FALSE;
	}
}

BOOL  CMouseHook::StopHook()
{
	base::Log(9,"StopHook");
	if(m_hHook )
	{
		if(UnhookWindowsHookEx(m_hHook))
		{
			m_hHook = NULL;
			m_hWnd = NULL;
			base::Log(9,"StopHook successed");
		} else
		{
			 ErrorMsg();
			 return FALSE;
		}		
	} 
	return TRUE;
}