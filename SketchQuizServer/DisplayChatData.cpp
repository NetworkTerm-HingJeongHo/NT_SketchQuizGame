#include "stdafx.h"

void InitializeChatListView(HWND hWnd) {
    g_hChatListView = CreateWindowEx(0, WC_LISTVIEW, _T("Chat Data"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        220, 10, 350, 240, hWnd, NULL, NULL, NULL);

    // 첫 번째 열 추가 (아이디)
    LVCOLUMN lvc1;
    lvc1.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc1.cx = 100;  // 첫 번째 열의 너비 설정
    lvc1.pszText = (LPWSTR)_T("ID");  // 첫 번째 열에 "Time" 표시
    ListView_InsertColumn(g_hChatListView, 0, &lvc1);

    // 두 번째 열 추가 (메시지)
    LVCOLUMN lvc2;
    lvc2.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc2.cx = 250;  // 두 번째 열의 너비 설정
    lvc2.pszText = (LPWSTR)_T("Message");  // 두 번째 열에 "Message" 표시
    ListView_InsertColumn(g_hChatListView, 1, &lvc2);
}

void AddChatMessageToListView(_TCHAR* id, _TCHAR* message) {
    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT | LVIF_PARAM;
    lvItem.iItem = 0;
    lvItem.iSubItem = 0;

    // 아이디 추가
    lvItem.pszText = id;

    // 첫 번째 열에 아이디 추가
    int index = ListView_InsertItem(g_hChatListView, &lvItem);

    // 두 번째 열에 메시지 추가
    ListView_SetItemText(g_hChatListView, index, 1, message);
    DisplayChatList();
}

// 채팅 데이터 목록을 윈도우 창에 표시
void DisplayChatList() {
    // 채팅 데이터가 변경되었을 때 호출하여 리스트 뷰를 업데이트합니다.
    ListView_RedrawItems(g_hChatListView, 0, ListView_GetItemCount(g_hChatListView) - 1);
    UpdateWindow(g_hChatListView);
}