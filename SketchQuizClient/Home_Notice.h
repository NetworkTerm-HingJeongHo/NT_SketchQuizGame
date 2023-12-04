#pragma once
#include "stdafx.h"

LRESULT CALLBACK Home_NoticeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateAndShowWindow_Home_Notice(HWND hwnd);
int BoradcaseSendToNotice(_TCHAR* input_notice_result);