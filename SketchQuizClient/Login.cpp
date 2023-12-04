#include "stdafx.h"



// 로그인 창을 만들고 표시하는 함수
void CreateAndShowWindow_Login(HWND hWnd)
{
	// 로그인 창 생성
	hwndLogin = CreateWindow(_T("LoginWindowClass"), _T("로그인 창"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, g_hInstance, NULL);
	ShowWindow(hwndLogin, SW_SHOW);
}
