#include "DxLib.h"

#include <stdlib.h>		
#include <tchar.h>		

#define MY_WIN_CLASS	TEXT("MY_WIN_CLASS")		
#define MY_WIN_TITLE	TEXT("ブロック崩し")	

#define MY_WINDOW_WIDTH		800		
#define MY_WINDOW_HEIGHT	600		
#define FPS_VALUE			60		


#define KEYCODE_MAX	256				




enum GAME_SCENE
{
	scene_start,	
	scene_play,		
	scene_end		
};


WNDCLASS winc;		
HWND hWnd;			
MSG msg;			
RECT rect_window;	
RECT rect_client;	
POINT mousePt;		
HINSTANCE AppInst;	
float		fps;		

DWORD		fps_sta_tm;	
DWORD		fps_end_tm;
DWORD		fps_count;	

HBITMAP hbmp_double;	
HDC hdc_double;			


BYTE KeyBoard[KEYCODE_MAX];			
BOOL KeyDownState[KEYCODE_MAX];		

int GameScene = (int)scene_start;	


BOOL IsWindowClassRegi(PCTSTR, HINSTANCE);				
VOID IsWindowCreate(HWND*, PCTSTR, HINSTANCE);			
VOID SetClientSize(HWND hWnd);						
VOID CreateDoubleBufferring(HWND);					

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);	

VOID MY_FPS_UPDATE(VOID);			
VOID MY_FPS_DRAW(HDC, int, int);	
VOID MY_FPS_WAIT(VOID);				


VOID MY_UPDATE_KEYDOWN(VOID);		

VOID MY_GAME_PROC(VOID);	
VOID MY_GAME_DRAW(HDC);		

VOID MY_GAME_PROC_START(VOID);	
VOID MY_GAME_PROC_PLAY(VOID);	
VOID MY_GAME_PROC_END(VOID);	

VOID MY_GAME_DRAW_START(HDC);	
VOID MY_GAME_DRAW_PLAY(HDC);	
VOID MY_GAME_DRAW_END(HDC);		


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	AppInst = hInstance;

	if (IsWindowClassRegi(MY_WIN_CLASS, hInstance) == FALSE) { exit(EXIT_FAILURE); }

	IsWindowCreate(&hWnd, MY_WIN_CLASS, hInstance);	if (hWnd == NULL) { exit(EXIT_FAILURE); }

	SetClientSize(hWnd);

	ShowWindow(hWnd, nCmdShow);

	CreateDoubleBufferring(hWnd);

	BOOL PeekKekka = FALSE;	

	while (TRUE)	
	{
		PeekKekka = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (PeekKekka == TRUE)		
		{
			if (msg.message == WM_QUIT) { break; }
			DispatchMessage(&msg);	
		}
		else if (PeekKekka == FALSE)
		{
			MY_FPS_UPDATE();


			
			MY_UPDATE_KEYDOWN();

			MY_GAME_PROC();


			InvalidateRect(hWnd, NULL, FALSE);

			UpdateWindow(hWnd);

			MY_FPS_WAIT();
		}
	}

	DestroyWindow(hWnd);

	
	UnregisterClass(MY_WIN_CLASS, hInstance);
	return msg.wParam;	
}


BOOL IsWindowClassRegi(PCTSTR wndclass, HINSTANCE inst)
{
	winc.style =
		CS_HREDRAW							
		| CS_VREDRAW;						
	  //| CS_NOCLOSE						
	winc.lpfnWndProc = WinProc;				
	winc.cbClsExtra = 0;		
	winc.cbWndExtra = 0;		
	winc.hInstance = inst;		
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);					
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;									
	winc.lpszClassName = wndclass;								

	if (!RegisterClass(&winc)) { return FALSE; }

	return TRUE;
}


VOID IsWindowCreate(HWND* wnd, PCTSTR wndclass, HINSTANCE inst)
{
	*(wnd) = CreateWindowEx(
		WS_EX_LEFT,							
		wndclass,							
		MY_WIN_TITLE,						
		WS_CAPTION							
		| WS_SYSMENU						
		| WS_MINIMIZEBOX					
		| WS_VISIBLE,						
		CW_USEDEFAULT,						
		CW_USEDEFAULT,					
		MY_WINDOW_WIDTH,					
		MY_WINDOW_HEIGHT,					
		NULL,								
		NULL,								
		inst,								
		NULL								
	);

	return;
}


VOID SetClientSize(HWND hWnd)
{
	RECT rect_set;	

	rect_set.top = 0;					
	rect_set.left = 0;					
	rect_set.right = MY_WINDOW_WIDTH;	
	rect_set.bottom = MY_WINDOW_HEIGHT;	

	GetWindowRect(hWnd, &rect_window);	
	GetClientRect(hWnd, &rect_client);	

	int WindowWidth = rect_window.right - rect_window.left;
	int ClientWidth = rect_client.right - rect_client.left;	
	int YohakuWidth = WindowWidth - ClientWidth;			
	rect_set.right += YohakuWidth;							

	int WindowHeight = rect_window.bottom - rect_window.top;
	int ClientHeight = rect_client.bottom - rect_client.top;
	int YohakuHeight = WindowHeight - ClientHeight;				
	rect_set.bottom += YohakuHeight;							

	SetWindowPos(
		hWnd,					
		HWND_TOP,				
		rect_set.left,			
		rect_set.top,			
		rect_set.right,			
		rect_set.bottom,		
		SWP_SHOWWINDOW);		

	GetClientRect(hWnd, &rect_client);	
	return;
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int retCloseMsg;	

	HDC hdc;							

	PAINTSTRUCT ps;					

	switch (msg)
	{
	case WM_CREATE:	

		return 0;		

	case WM_MOUSEMOVE:	
		mousePt.y = HIWORD(lp);	
		mousePt.x = LOWORD(lp);	

		return 0;

	case WM_LBUTTONDOWN:	
		mousePt.y = HIWORD(lp);	
		mousePt.x = LOWORD(lp);	

		return 0;

	case WM_LBUTTONUP:
		mousePt.y = HIWORD(lp);	
		mousePt.x = LOWORD(lp);	

		return 0;

	case WM_PAINT:	

		
		hdc = BeginPaint(hwnd, &ps);	
		SaveDC(hdc);					

		
	
		Rectangle(hdc_double,
			rect_client.left, rect_client.top, rect_client.right, rect_client.bottom);

		MY_FPS_DRAW(hdc_double, 10, MY_WINDOW_HEIGHT - 20);				

		

		MY_GAME_DRAW(hdc_double);

		

		
		BitBlt(hdc, 0, 0, rect_client.right, rect_client.bottom, hdc_double, 0, 0, SRCCOPY);

		RestoreDC(hdc, -1);		

		EndPaint(hwnd, &ps);	

		return 0;

	case WM_CLOSE:		
		retCloseMsg =
			MessageBox(hwnd, TEXT("終了しますか？"), TEXT("Close?"), MB_YESNO | MB_ICONINFORMATION);

		if (retCloseMsg == IDNO) { return 0; }

		break;

	case WM_DESTROY:	
		PostQuitMessage(0);	

		DeleteObject(hbmp_double);	
		DeleteDC(hdc_double);		

		return 0;

	}
	return DefWindowProc(hwnd, msg, wp, lp);	
}


VOID CreateDoubleBufferring(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);

	hbmp_double = CreateCompatibleBitmap(hdc, rect_client.right, rect_client.bottom);

	hdc_double = CreateCompatibleDC(hdc);	
	SelectObject(hdc_double, hbmp_double);

	ReleaseDC(hWnd, hdc);	
	return;
}

VOID MY_FPS_UPDATE(VOID)
{
	if (fps_count == 0)	
	{
		fps_sta_tm = GetTickCount();	
	}

	if (fps_count == FPS_VALUE)	
	{
		fps_end_tm = GetTickCount();	
		fps = 1000.0f / ((fps_end_tm - fps_sta_tm) / (float)FPS_VALUE);	
		fps_sta_tm = fps_end_tm;		
		fps_count = 0;
	}
	fps_count++;	

	return;
}


VOID MY_FPS_DRAW(HDC hdc, int drawX, int drawY)
{
	TCHAR DrawStrFPS[64];		
	_stprintf(DrawStrFPS, TEXT("FPS：%.1f"), fps);

	SetTextColor(hdc, RGB(0, 0, 0));		
	SetBkColor(hdc, RGB(255, 255, 255));	
	SetBkMode(hdc, TRANSPARENT);			
	TextOut(hdc_double, drawX, drawY, DrawStrFPS, lstrlen(DrawStrFPS));
	return;
}

VOID MY_FPS_WAIT(VOID)
{
	DWORD now_tm = GetTickCount();

	DWORD keika_tm = now_tm - fps_sta_tm;

	DWORD wait_tm = (fps_count * 1000 / FPS_VALUE) - (keika_tm);

	if (wait_tm > 0 && wait_tm < 1000) { Sleep(wait_tm); }	
	return;
}


VOID MY_UPDATE_KEYDOWN(VOID)
{
	int checkKey;	

	GetKeyboardState(KeyBoard);		

	BYTE IskeyDown_W = KeyBoard['W'] & 0x80;	
	BYTE IskeyDown_D = KeyBoard['D'] & 0x80;	
	BYTE IskeyDown_A = KeyBoard['A'] & 0x80;	
	BYTE IskeyDown_S = KeyBoard['S'] & 0x80;	

	for (checkKey = 0; checkKey < KEYCODE_MAX; checkKey++)
	{
		if ((KeyBoard[checkKey] & 0x80) != 0)
		{
			KeyDownState[checkKey] = TRUE;	
		}
		else
		{
			KeyDownState[checkKey] = FALSE;	
		}
	}

	return;
}


VOID MY_GAME_PROC(VOID)
{
	
	switch (GameScene)
	{
	case (int)scene_start:	
		MY_GAME_PROC_START();
		break;

	case (int)scene_play:	
		MY_GAME_PROC_PLAY();
		break;

	case (int)scene_end:	
		MY_GAME_PROC_END();
		break;
	}

	return;
}


VOID MY_GAME_DRAW(HDC hdc)
{
	switch (GameScene)
	{
	case (int)scene_start:
		MY_GAME_DRAW_START(hdc);
		break;

	case (int)scene_play:	
		MY_GAME_DRAW_PLAY(hdc);
		break;

	case (int)scene_end:	
		MY_GAME_DRAW_END(hdc);
		break;
	}

	return;
}

VOID MY_GAME_PROC_START(VOID)
{
	if (KeyDownState[VK_SPACE] == TRUE)	
	{
		

		GameScene = (int)scene_play;	
	}

	return;
}

VOID MY_GAME_PROC_PLAY(VOID)
{
	if (KeyDownState[VK_RETURN] == TRUE)
	{
		

		GameScene = (int)scene_end;	
	}

	return;
}

VOID MY_GAME_PROC_END(VOID)
{
	if (KeyDownState[VK_ESCAPE] == TRUE)	
	{
		

		GameScene = (int)scene_start;	
	}
	return;
}

VOID MY_GAME_DRAW_START(HDC hdc)
{
	TCHAR DrawString[128];		
	wsprintf(DrawString, TEXT("スタート画面（スペースキーを押してください）"));
	SetTextColor(hdc, RGB(0, 0, 0));	
	SetBkColor(hdc, RGB(0, 0, 0));		
	SetBkMode(hdc, TRANSPARENT);		

	TextOut(hdc, 10, 0, DrawString, lstrlen(DrawString));

	return;
}

VOID MY_GAME_DRAW_PLAY(HDC hdc)
{
	TCHAR DrawString[128];		
	wsprintf(DrawString, TEXT("プレイ画面（エンターキーを押してください）"));
	SetTextColor(hdc, RGB(0, 0, 0));	
	SetBkColor(hdc, RGB(0, 0, 0));		
	SetBkMode(hdc, TRANSPARENT);		

	TextOut(hdc, 10, 0, DrawString, lstrlen(DrawString));

	return;
}

VOID MY_GAME_DRAW_END(HDC hdc)
{
	TCHAR DrawString[128];		
	wsprintf(DrawString, TEXT("エンド画面（エスケープキーを押してください）"));
	SetTextColor(hdc, RGB(0, 0, 0));	
	SetBkColor(hdc, RGB(0, 0, 0));		
	SetBkMode(hdc, TRANSPARENT);		

	TextOut(hdc, 10, 0, DrawString, lstrlen(DrawString));

	return;
}
