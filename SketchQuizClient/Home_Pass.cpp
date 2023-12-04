#include "stdafx.h"

// 패스워드 입력 창을 만들고 표시하는 함수
void CreateAndShowWindow_Home_Pass(HWND hWnd)
{
	// 로그인 창 생성
	hwndHome_Pass = CreateWindow(_T("Home_PassWindowClass"), _T("패스워드 입력 창"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, g_hInstance, NULL);
	ShowWindow(hwndHome_Pass, SW_SHOW);
}
