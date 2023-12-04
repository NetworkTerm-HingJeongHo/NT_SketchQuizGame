#include "stdafx.h"



// 홈 창을 만들고 표시하는 함수
void CreateAndShowWindow_Home(HWND hWnd)
{
	// 홈 창 생성
	hwndHome = CreateWindow(_T("MainWindowClass"), _T("메인 화면"), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, g_hInstance, NULL);
	ShowWindow(hwndHome, SW_SHOW);
}
