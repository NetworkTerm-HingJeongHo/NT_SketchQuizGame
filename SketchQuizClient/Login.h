#pragma once
#include "stdafx.h"

void CreateAndShowWindow_Login(HWND hwnd);
LRESULT CALLBACK LoginWndProc(HWND hwndLogin, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI LoginProcessClient(LPVOID arg);
