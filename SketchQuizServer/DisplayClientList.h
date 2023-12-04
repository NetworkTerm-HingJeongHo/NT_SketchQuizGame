#pragma once

static HWND g_hListView;

void InitializeListView(HWND hWnd);
void AddClientToListView(int port, const char* id);
void DisplayClientList();
void RemoveClientFromListView(int port);