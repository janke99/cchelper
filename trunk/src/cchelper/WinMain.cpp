// cchess_tool.cpp : Defines the entry point for the application.
//
#include "common.h"
#include "Resource.h"
#include "QQNewChessWnd.h"
#include "fastdib.h"
#include "app.h"

// STRUCTS
//___________________________________________________________________________
struct WindowInformation
{
	HWND hwnd;
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	WINDOWINFO wi;
} g_WinInfo;


// Forward declarations of functions included in this code module:
//___________________________________________________________________________
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID GetWindowInformation();


// GLOBAL VARIABLES DECLARATIONS
//___________________________________________________________________________
POINT		g_ptMouse;
int			g_intAppRunning;
BOOL		g_bActive;
HWND		g_hWndMain;
BOOL		g_bInitialized	= FALSE;
CFastDIB	*g_pMainSurface = NULL;


HINSTANCE	hInst;								// current instance
TCHAR		szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR		szWindowClass[MAX_LOADSTRING];		// the main window class name

int g_x = 0, g_y = 0;				// for test
COLORREF * g_pBoardRectPic = NULL;	// for test


//FUNCTIONS
//___________________________________________________________________________
void SetWindowSize(DWORD dwWidth,DWORD dwHeight)
{
	RECT  rc;

	// Ajust window size
	SetRect( &rc, 0, 0, dwWidth, dwHeight );

	AdjustWindowRectEx( &rc, GetWindowStyle(g_hWndMain), GetMenu(g_hWndMain) != NULL,
						GetWindowExStyle(g_hWndMain) );

	MoveWindow(g_hWndMain,
		rc.left+(GetSystemMetrics(SM_CXSCREEN)-dwWidth)/2,
		rc.top+(GetSystemMetrics(SM_CYSCREEN)-dwHeight)/2,
		rc.right-rc.left,
		rc.bottom-rc.top,
		FALSE );

	CFastDIB::Initialize(dwWidth, dwHeight);

	g_pMainSurface = new CFastDIB();

	_CHECK(g_pMainSurface->CreateDIB( dwWidth, dwHeight, FDIBTYPE_RGBA|FDIBTYPE_DOUBLEBUF ));

}



void AlreadyRun(void)
{
	HWND	FirsthWnd, FirstChildhWnd;

	if((FirsthWnd = FindWindowEx(NULL, NULL, szWindowClass, NULL )) != NULL)
	{
		FirstChildhWnd = GetLastActivePopup(FirsthWnd);
		SetForegroundWindow(FirsthWnd);
	
		if(FirsthWnd != FirstChildhWnd)
		{
			SetForegroundWindow(FirstChildhWnd);
		}
		
		ShowWindow(FirsthWnd, SW_SHOWNORMAL);
	}
}


BOOL doInit(HINSTANCE hInstance, int nCmdShow)
{
	HACCEL hAccelTable;

	MyRegisterClass(hInstance);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCHELPER));

    //  create a window
    g_hWndMain = CreateWindowEx(WS_EX_TOPMOST,
                          szWindowClass,
                          szTitle,
                          WS_OVERLAPPEDWINDOW,
                          0,
                          0,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    if(!g_hWndMain) return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCHELPER));

	SetWindowSize(210, 230);

	if(!InitApp())
		return FALSE;

	g_bInitialized = TRUE;

    ShowWindow(g_hWndMain, nCmdShow);
    UpdateWindow(g_hWndMain);

    return TRUE;
} 

BOOL doUpdate()
{
	HDC hdc = GetDC(g_hWndMain);

	if ( g_pMainSurface && hdc )
	{
		g_pMainSurface->FastBlt(hdc);

		ReleaseDC(g_hWndMain, hdc);
	}
	return TRUE;
}

BOOL doRelease()
{
	g_intAppRunning = FALSE;

	if ( g_pMainSurface ) 
	{
		delete g_pMainSurface;
		g_pMainSurface = NULL;
	}
	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CCHELPER, szWindowClass, MAX_LOADSTRING);

	HANDLE	hMutex;

	hMutex = CreateMutex(NULL, TRUE, szWindowClass);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		AlreadyRun();
		return 0;
	}

	if( !doInit(hInstance, nCmdShow) )
		return -1;

    for (;;)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
        {
            if(!GetMessage(&msg,NULL,0,0))
            {
                return msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if(g_bActive) {
			if (!AppLoop()) {
			    DestroyWindow(g_hWndMain);
			}else{
				doUpdate();
			}
		}
		else
        {
            WaitMessage();
        }
    }


	return (int) msg.wParam;
}

void GetWindowInformation()
{

	TCHAR buf[256];

	GetCursorPos (&g_ptMouse);

	memset(&g_WinInfo, 0, sizeof(g_WinInfo));

	g_WinInfo.hwnd  = WindowFromPoint(g_ptMouse);

	RealGetWindowClass(g_WinInfo.hwnd , g_WinInfo.szWindowClass, sizeof(g_WinInfo.szWindowClass));
	GetWindowInfo(g_WinInfo.hwnd , &g_WinInfo.wi );

	int id = GetDlgCtrlID(g_WinInfo.hwnd );

	HWND hwndEdit = GetDlgItem(g_hWndMain,ID_EDITCHILD);

	_stprintf_s(buf, _T("「WindowClass」=%s 「CtrlID」=0x%p 「Location」=%d,%d"), g_WinInfo.szWindowClass, id, 
		g_WinInfo.wi.rcWindow.right - g_WinInfo.wi.rcWindow.left  , 
		g_WinInfo.wi.rcWindow.bottom - g_WinInfo.wi.rcWindow.top);

	// Add text to the window. 
	//SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) buf);

	InvalidateRect(g_hWndMain,NULL,FALSE);
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CCHELPER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CCHELPER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

void 	DrawBoardRectPic(HDC hdc)
{
	if ( g_pBoardRectPic )
	{
		for( int i =0; i < 58; i ++ )
			for ( int j = 0; j < 58 ; j ++ )
			{
				SetPixel(hdc, 100 + i, 200 + j , g_pBoardRectPic[j * 58 + i]);
			}
	}
}

LRESULT CALLBACK WndProc(HWND g_hWndMain, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int vk;

	switch (message)
	{
	case WM_ACTIVATEAPP:
		g_bActive = wParam;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), g_hWndMain, About);			
			break;
		case IDM_EXIT:
			DestroyWindow(g_hWndMain);
			break;
		default:
			return DefWindowProc(g_hWndMain, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(g_hWndMain, &ps);

		if ( g_pMainSurface  )
		{
			g_pMainSurface->FastBlt(hdc);
		}

		EndPaint(g_hWndMain, &ps);
		break;
	case WM_KEYUP:
		//if ( g_pBoardRectPic )
		//{
		//	delete g_pBoardRectPic ;
		//	g_pBoardRectPic = NULL;
		//}

		//if ( g_pQcnWnd &&  g_pQcnWnd->GetHwnd() ) 
		//{
		//	vk = (int) wParam;

		//	switch( vk )
		//	{
		//	case VK_F2:
		//		g_pQcnWnd->dx --;
		//		break;
		//	case VK_F3:
		//		g_pQcnWnd->dx ++;
		//		break;

		//	case VK_F4:
		//		g_pQcnWnd->dy --;
		//		break;
		//	case VK_F5:
		//		g_pQcnWnd->dy ++;
		//		break;

		//	case  VK_LEFT:
		//		g_pQcnWnd->ox --;
		//		break;
		//	case VK_RIGHT:
		//		g_pQcnWnd->ox ++;
		//		break;
		//	case VK_UP:
		//		g_pQcnWnd->oy --;
		//		break;
		//	case VK_DOWN:
		//		g_pQcnWnd->oy ++;
		//		break;
		//	case VK_SPACE:
		//		g_x =(int)( (double)rand() / (RAND_MAX + 1) * 9 );
		//		g_y =(int)( (double)rand() / (RAND_MAX + 1) * 10 );
		//		break;
		//	case VK_RETURN:
		//		DumpBoradHash();
		//		break;
		//	}
		//	g_pBoardRectPic = g_pQcnWnd->GetRectData( g_x, g_y) ;
		//	InvalidateRect(g_hWndMain,NULL,FALSE);
		//}
		break;

	//case WM_TIMER:
	//	switch( wParam )
	//	{
	//	case 1:
	//		if( g_pQcnWnd )
	//		{
	//			if (g_pQcnWnd->GetHwnd() )
	//			{
	//				SetWindowText(g_hWndMain, _T("Helping"));
	//			} 
	//			else
	//			{
	//				SetWindowText(g_hWndMain, _T("Searching"));
	//			}
	//		}
	//		return 0;
	//	}
	//	break;
	case WM_DESTROY:
		if(g_bInitialized)
		{
			ExitApp();
		}

		doRelease();

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(g_hWndMain, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

