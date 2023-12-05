#include "stdafx.h"

void InitializeChatListView(HWND hWnd) {
    g_hChatListView = CreateWindowEx(0, WC_LISTVIEW, _T("Chat Data"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        220, 10, 350, 240, hWnd, NULL, NULL, NULL);

    // 첫 번째 열 추가
    LVCOLUMN lvc1;
    lvc1.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc1.cx = 350;  // 첫 번째 열의 너비 설정
    lvc1.pszText = (LPWSTR)_T("Chat Data"); 
    ListView_InsertColumn(g_hChatListView, 0, &lvc1);
}

void AddChatMessageToListView(char* message) {
    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT | LVIF_PARAM;
    lvItem.iItem = 0;
    lvItem.iSubItem = 0;

    // 문자열의 길이 계산
    int length = strlen(message) + 1;

    // 필요한 버퍼를 할당하고 문자열을 와이드 문자열로 변환
    wchar_t* portStrW = (wchar_t*)malloc(length * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, message, -1, portStrW, length);

    lvItem.pszText = portStrW;

    ListView_InsertItem(g_hChatListView, &lvItem);
    DisplayChatList();
}

// 채팅 데이터 목록을 윈도우 창에 표시
void DisplayChatList() {
    // 채팅 데이터가 변경되었을 때 호출하여 리스트 뷰를 업데이트합니다.
    ListView_RedrawItems(g_hChatListView, 0, ListView_GetItemCount(g_hChatListView) - 1);
    UpdateWindow(g_hChatListView);
}

void ClearChatListView() {
    ListView_DeleteAllItems(g_hChatListView);
    DisplayChatList();
}