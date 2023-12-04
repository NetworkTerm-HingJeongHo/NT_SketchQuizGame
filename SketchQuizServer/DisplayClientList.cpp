#include "stdafx.h"

void InitializeListView(HWND hWnd) {
    g_hListView = CreateWindowEx(0, WC_LISTVIEW, _T("Client List"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        10, 10, 200, 320, hWnd, NULL, NULL, NULL);

    // 첫 번째 열 추가
    LVCOLUMN lvc1;
    lvc1.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc1.cx = 100;  // 첫 번째 열의 너비 설정
    lvc1.pszText = (LPWSTR)_T("Port");  // 첫 번째 열에 "Port" 표시
    ListView_InsertColumn(g_hListView, 0, &lvc1);

    // 두 번째 열 추가
    LVCOLUMN lvc2;
    lvc2.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc2.cx = 100;  // 두 번째 열의 너비 설정
    lvc2.pszText = (LPWSTR)_T("User ID");  // 두 번째 열에 "User ID" 표시
    ListView_InsertColumn(g_hListView, 1, &lvc2);
}

void AddClientToListView(int port, const char* id) {
    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT | LVIF_PARAM;
    lvItem.iItem = 0;
    lvItem.iSubItem = 0;

    // 포트 번호를 문자열로 변환하여 추가
    wchar_t portStrW[10];
    swprintf(portStrW, sizeof(portStrW) / sizeof(portStrW[0]), _T("%d"), port);
    lvItem.pszText = portStrW;
    lvItem.lParam = port; // 포트 번호 저장

    // 첫 번째 열에 포트 번호 추가
    int index = ListView_InsertItem(g_hListView, &lvItem);

    // ID를 wchar_t로 변환
    wchar_t idStrW[256];
    mbstowcs(idStrW, id, strlen(id) + 1);

    // 두 번째 열에 ID 추가
    ListView_SetItemText(g_hListView, index, 1, idStrW);
}

// 클라이언트 목록을 윈도우 창에 표시
void DisplayClientList() {
    // 클라이언트 정보가 변경되었을 때 호출하여 리스트 뷰를 업데이트합니다.
    ListView_RedrawItems(g_hListView, 0, ListView_GetItemCount(g_hListView) - 1);
    UpdateWindow(g_hListView);
}

void RemoveClientFromListView(int port) {
    LVFINDINFO lvFindInfo;
    lvFindInfo.flags = LVFI_PARAM;

    // 파라미터로 포트 번호를 검색에 사용
    lvFindInfo.lParam = port;

    int index = ListView_FindItem(g_hListView, -1, &lvFindInfo);
    if (index != -1) {
        ListView_DeleteItem(g_hListView, index);
    }
}