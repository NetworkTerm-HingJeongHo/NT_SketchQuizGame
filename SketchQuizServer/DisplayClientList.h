#pragma once

#define CLIENTOUT 3001

static HWND g_hListView;

void InitializeListView(HWND hWnd);
void AddClientToListView(int port, _TCHAR* id);
void DisplayClientList();
void RemoveClientFromListView(int port);