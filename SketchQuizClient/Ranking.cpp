#include "stdafx.h"


void CreateRankDlg(HWND hWnd)
{
	g_hRankDlg = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgRanking);
	ShowWindow(g_hRankDlg, SW_SHOW);
	//g_bDialogVisible = true;
}

INT_PTR CALLBACK DlgRanking(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hButtonExit;
	static HWND hButtonStay;

	static HWND hRank1;
	static HWND hRank2;
	static HWND hRank3;
	static HWND hMyRank;

	switch (uMsg) {
	case WM_INITDIALOG:
		// 컨트롤 핸들 얻기
		hButtonExit = GetDlgItem(hDlg, IDC_EXIT);
		hButtonStay = GetDlgItem(hDlg, IDOK);
		hRank1 = GetDlgItem(hDlg, IDC_EDIT1ST);
		hRank2 = GetDlgItem(hDlg, IDC_EDIT2ND);
		hRank3 = GetDlgItem(hDlg, IDC_EDIT3RD);
		hMyRank = GetDlgItem(hDlg, IDC_EDIT_MYID);

		// 컨트롤 초기화
		SendMessage(hRank1, EM_SETLIMITTEXT, MSGSIZE, 0);
		SendMessage(hRank2, EM_SETLIMITTEXT, MSGSIZE, 0);
		SendMessage(hRank3, EM_SETLIMITTEXT, MSGSIZE, 0);
		SendMessage(hMyRank, EM_SETLIMITTEXT, MSGSIZE, 0);

		//// 윈도우 클래스 등록
		//WNDCLASS wndclass;
		//wndclass.style = CS_HREDRAW | CS_VREDRAW;
		//wndclass.lpfnWndProc = WndProc;
		//wndclass.cbClsExtra = 0;
		//wndclass.cbWndExtra = 0;
		//wndclass.hInstance = g_hInst;
		//wndclass.lpszClassName = "MyRankingClass";
		//if (!RegisterClass(&wndclass)) return 1;


		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_EXIT:
			EndDialog(hDlg, IDC_EXIT);
			closesocket(g_sock);
			//EndDialog(g_hChattingDlg, IDC_EXIT);
			//ShowWindow(g_hChattingDlg, SW_HIDE);
			//DestroyWindow(g_hChattingDlg);
			return TRUE;
		case IDOK:
			EndDialog(hDlg, IDOK);
			//CreateAndShowDialog(g_hChattingDlg);
			return FALSE;

		}
		return FALSE;
	}

	return FALSE;
}