#pragma once

// 클라이언트 목록 view 핸들
static HWND g_hListView;

void InitializeListView(HWND hWnd);
void AddClientToListView(int port, _TCHAR* id, SOCKET sock);
void DisplayClientList();
void RemoveClientFromListView(int port);