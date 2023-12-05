#include "stdafx.h"

void InitializeListView(HWND hWnd) {
    g_hListView = CreateWindowEx(0, WC_LISTVIEW, _T("Client List"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        10, 10, 200, 200, hWnd, NULL, NULL, NULL);

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

    // ListView의 선택 모드를 활성화
    ListView_SetExtendedListViewStyle(g_hListView, LVS_EX_FULLROWSELECT);
}

// 클라이언트 목록 인덱스와 해당 소켓 저장
void AddClientSocketAndIndex(int index, SOCKET sock) {
    tmp_ClientSock[index] = sock;
}

void AddClientToListView(int port, _TCHAR* id, SOCKET sock) {
    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT | LVIF_PARAM;
    lvItem.iItem = 0;
    lvItem.iSubItem = 0;

    // 포트 번호를 문자열로 변환하여 추가
    wchar_t portStrW[10];
    swprintf(portStrW, sizeof(portStrW) / sizeof(portStrW[0]), _T("%d"), port);
    lvItem.pszText = portStrW;

    // 첫 번째 열에 포트 번호 추가
    int index = ListView_InsertItem(g_hListView, &lvItem);

    // 두 번째 열에 ID 추가
    ListView_SetItemText(g_hListView, index, 1, id);

    // 클라이언트 목록 인덱스와 해당 소켓 저장
    AddClientSocketAndIndex(index, sock);

    DisplayClientList();
}

// 클라이언트 목록을 윈도우 창에 표시
void DisplayClientList() {
    // 클라이언트 정보가 변경되었을 때 호출하여 리스트 뷰를 업데이트합니다.
    ListView_RedrawItems(g_hListView, 0, ListView_GetItemCount(g_hListView) - 1);
    UpdateWindow(g_hListView);
}

// 포트 번호에 해당하는 클라이언트를 목록에서 제거
void RemoveClientFromListView(int port) {
    LVFINDINFO lvFindInfo;
    lvFindInfo.flags = LVFI_STRING;

    // 포트 번호를 문자열로 변환하여 검색에 사용
    wchar_t portStrW[10];
    swprintf(portStrW, sizeof(portStrW) / sizeof(portStrW[0]), _T("%d"), port);
    lvFindInfo.psz = portStrW;

    int index = ListView_FindItem(g_hListView, -1, &lvFindInfo);
    if (index != -1) {
        ListView_DeleteItem(g_hListView, index);
    }
    DisplayClientList();
}

void RemoveClientFromListViewAndSock(int index) {
    RemoveSocketInfo(tmp_ClientSock[index]);
}