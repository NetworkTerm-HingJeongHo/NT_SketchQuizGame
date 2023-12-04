#pragma once

// 클라이언트 목록 리스트 핸들
static HWND g_hListView;
// 선택한 클라이언트의 인덱스를 저장할 전역 변수
static int g_selectedIndex = -1;
// 임시 소켓 저장 변수
static SOCKET tmp_clientSock;

void InitializeListView(HWND hWnd);
void AddClientToListView(int port, _TCHAR* id);
void DisplayClientList();
void RemoveClientFromListView(int port);