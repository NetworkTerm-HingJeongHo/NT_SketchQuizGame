#pragma once

// 채팅 데이터 view 핸들
static HWND g_hChatListView;

void InitializeChatListView(HWND hWnd);
void AddChatMessageToListView(_TCHAR* message);
void DisplayChatList();