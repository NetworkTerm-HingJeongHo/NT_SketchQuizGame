#pragma once

// 클라이언트 목록 view 핸들
static HWND g_hListView;
// 선택한 클라이언트의 인덱스를 저장할 전역 변수
static int g_selectedIndex = -1;
// 임시 저장 소켓
static SOCKET tmp_ClientSock[10];

void InitializeListView(HWND hWnd);
void AddClientSocketAndIndex(int index, SOCKET sock);
void AddClientToListView(int port, _TCHAR* id, SOCKET sock);
void DisplayClientList();
void RemoveClientFromListView(int port);
void RemoveClientFromListViewAndSock(int index);