#pragma once

#define BUFSIZE     256                    // 전송 메시지 전체 크기
#define MSGSIZE     (BUFSIZE-sizeof(int))  // 채팅 메시지 최대 길이

void CreateRankDlg(HWND hWnd);
INT_PTR CALLBACK DlgRanking(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);