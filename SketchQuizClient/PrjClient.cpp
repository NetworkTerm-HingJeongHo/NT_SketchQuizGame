#include "stdafx.h"

// ---- 지안 변수 (로그인을 위함) ----- //
_TCHAR input_result[256]; // input 결과 저장할 배열
_TCHAR ID_NICKNAME[256]; // stdafx.h 파일에 같은 주소에 저장하기 위함
HANDLE LoginProcessClientThread; // 로그인 프로세스 스레드, stdafx.h 파일에 같은 주소에 저장하기 위함
char recvBuf[BUFSIZE]; // 데이터 받을 버퍼
_TCHAR recvBuf_tchar[BUFSIZE]; // tchar로 받을 버퍼
// 홈 창 변수
int channel;	//udp 채널 가져오기. stdafx.h 파일에 같은 주소에 저장하기 위함

//-------------------------------//

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 이벤트 생성(각각 신호, 비신호 상태)
	g_hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (g_hReadEvent == NULL) return 1;
	g_hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_hWriteEvent == NULL) return 1;

	// 전역 변수 초기화(일부)
	g_chatmsg.type = TYPE_CHAT;
	g_drawlinemsg.type = TYPE_DRAWLINE;
	g_drawlinemsg.color = RGB(255, 0, 0);
	g_erasepicmsg.type = TYPE_ERASEPIC;

	// ===== 정호 =====
	g_drawellipsemsg.type = TYPE_DRAWELLIPSE;
	g_drawellipsemsg.color = RGB(255, 0, 0);
	//

	g_hInstance = hInstance;


	//-------지안--------//
	// 홈 메인화면 윈도우 클래스 등록
	WNDCLASS wcMain = { 0 };
	wcMain.lpfnWndProc = HomeWndProc;
	wcMain.hInstance = hInstance;
	wcMain.lpszClassName = _T("MainWindowClass");
	RegisterClass(&wcMain);

	// 로그인 윈도우 클래스 등록
	WNDCLASS wcLogin = { 0 };
	wcLogin.lpfnWndProc = LoginWndProc;
	wcLogin.hInstance = hInstance;
	wcLogin.lpszClassName = _T("LoginWindowClass");
	RegisterClass(&wcLogin);

	// 홈_공지사항_입력 윈도우 클래스 등록
	WNDCLASS wcHome_Notice = { 0 };
	wcHome_Notice.lpfnWndProc = Home_NoticeWndProc;
	wcHome_Notice.hInstance = hInstance;
	wcHome_Notice.lpszClassName = _T("Home_NoticeWindowClass");
	RegisterClass(&wcHome_Notice);

	// 홈_공지사항_비밀번호 입력 윈도우 클래스 등록
	WNDCLASS wcHome_Pass = { 0 };
	wcHome_Pass.lpfnWndProc = Home_PassWndProc;
	wcHome_Pass.hInstance = hInstance;
	wcHome_Pass.lpszClassName = _T("Home_PassWindowClass");
	RegisterClass(&wcHome_Pass);

	//------------------//
	
	// 메인 윈도우(첫 화면) 생성
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MainWndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("MainWindow"), NULL };
	RegisterClassEx(&wcex);
	g_hMainWindow = CreateWindow(_T("MainWindow"), _T("Main Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 200, NULL, NULL, hInstance, NULL);

	ShowWindow(g_hMainWindow, nCmdShow);
	UpdateWindow(g_hMainWindow);


	// 메시지 루프
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 이벤트 객체 제거
	CloseHandle(g_hReadEvent);
	CloseHandle(g_hWriteEvent);
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 메인 윈도우 프로시저
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// '그림판' 버튼 생성
		CreateWindow(_T("BUTTON"), _T("그림판"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 100, 30, hWnd, (HMENU)1, g_hInstance, NULL);

		// '로그인' 버튼 생성
		CreateWindow(_T("BUTTON"), _T("로그인"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 120, 10, 100, 30, hWnd, (HMENU)2, g_hInstance, NULL);

		// '랭킹' 버튼 생성 (세 번째로 위치)
		CreateWindow(_T("BUTTON"), _T("랭킹"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 230, 10, 100, 30, hWnd, (HMENU)4, g_hInstance, NULL);

		// '메인' 버튼 생성 (네 번째로 위치)
		CreateWindow(_T("BUTTON"), _T("메인"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 340, 10, 100, 30, hWnd, (HMENU)3, g_hInstance, NULL);
		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == 1) // '그림판' 버튼 클릭
		{
			CreateAndShowDialog(hWnd);
		}
		//---지안 ----//
		else if (LOWORD(wParam) == 2) // '로그인' 버튼 클릭
		{
			CreateAndShowWindow_Login(hwndLogin);
		}
		else if (LOWORD(wParam) == 3) // '메인' 버튼 클릭
		{
			CreateAndShowWindow_Home(hwndHome); // 메인 생성
		}
		//-----------//
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 대화상자 프로시저(그림판 창)
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hChkIsIPv6;
	static HWND hEditIPaddr;
	static HWND hEditPort;
	static HWND hChkIsUDP;
	static HWND hBtnConnect;
	static HWND hBtnSendFile; // 전역 변수에도 저장
	static HWND hBtnSendMsg; // 전역 변수에도 저장
	static HWND hEditMsg;
	static HWND hEditStatus; // 전역 변수에도 저장
	static HWND hBtnErasePic; // 전역 변수에도 저장
	static HWND hStaticDummy;

	// ========= 연경 =========
	static HWND hTimer;    // 타이머 표시 
	static HWND hWord;     // 제시어 표시
	static HWND hBtnGameStart; // 게임 시작 버튼

	// ========= 지윤 =========
	static HWND hBtnPenColor;
	static HWND hLineWidth;
	static HWND hDlgChannel;
	static HWND hDrawingTextId;
	static HWND hDrawingText;

	// ========= 정호 =========
	static HWND hFigureSelect;	// 그릴 도형 선택
	//

	switch (uMsg) {
	case WM_INITDIALOG:
		// 컨트롤 핸들 얻기
		hChkIsIPv6 = GetDlgItem(hDlg, IDC_ISIPV6);
		hEditIPaddr = GetDlgItem(hDlg, IDC_IPADDR);
		hEditPort = GetDlgItem(hDlg, IDC_PORT);
		hChkIsUDP = GetDlgItem(hDlg, IDC_ISUDP);
		hBtnConnect = GetDlgItem(hDlg, IDC_CONNECT);
		hBtnSendFile = GetDlgItem(hDlg, IDC_SENDFILE);
		g_hBtnSendFile = hBtnSendFile; // 전역 변수에 저장
		hBtnSendMsg = GetDlgItem(hDlg, IDC_SENDMSG);
		g_hBtnSendMsg = hBtnSendMsg; // 전역 변수에 저장
		hEditMsg = GetDlgItem(hDlg, IDC_MSG);
		hEditStatus = GetDlgItem(hDlg, IDC_STATUS);
		g_hEditStatus = hEditStatus; // 전역 변수에 저장
		hBtnErasePic = GetDlgItem(hDlg, IDC_ERASEPIC);
		g_hBtnErasePic = hBtnErasePic; // 전역 변수에 저장
		hStaticDummy = GetDlgItem(hDlg, IDC_DUMMY);
		


		// ========= 연경 =========
		g_hTimerStatus = GetDlgItem(hDlg, IDC_EDIT_TIMER);  // 타이머 표시하는 EditText 부분 
		g_hWordStatus = GetDlgItem(hDlg, IDC_EDIT_WORD);    // 제시어 표시하는 EditText 부분
		hBtnGameStart = GetDlgItem(hDlg, IDC_GAMESTART);
		EnableWindow(hBtnGameStart, FALSE);

		g_hDrawDlg = hDlg;
		WideCharToMultiByte(CP_ACP, 0, ID_NICKNAME, 256, NICKNAME_CHAR, 256, NULL, NULL); //_TCHAR 형 문자열을 char* 형 문자열로 변경

		// ========= 지윤 =========
		hBtnPenColor = GetDlgItem(hDlg, IDC_PENCOLOR);
		g_hBtnPenColor = hBtnPenColor; // 전역 변수에 저장

		hLineWidth = GetDlgItem(hDlg, IDC_LINEWIDTH);
		g_hLineWidth = hLineWidth; // 전역 변수에 저장

		hDrawingTextId = GetDlgItem(hDlg, IDC_DRAWINGTEXTID);
		g_hDrawingTextId = hDrawingTextId; // 전역 변수에 저장

		hDrawingText = GetDlgItem(hDlg, IDC_DRAWINGTEXT);
		g_hDrawingText = hDrawingText; // 전역 변수에 저장

		// ========= 정호 =========
		// 그릴 도형 선택하는 핸들러를 얻어서 전역 변수에 저장
		hFigureSelect = GetDlgItem(hDlg, IDC_FIGURE);
		g_hFigureSelect = hFigureSelect;
		//

		// 컨트롤 초기화
		SetDlgItemText(hDlg, IDC_IPADDR, SERVERIP4);
		SetDlgItemInt(hDlg, IDC_PORT, SERVERPORT, FALSE);
		EnableWindow(g_hBtnSendFile, FALSE);
		EnableWindow(g_hBtnSendMsg, FALSE);
		SendMessage(hEditMsg, EM_SETLIMITTEXT, SIZE_DAT / 2, 0);
		EnableWindow(g_hBtnErasePic, FALSE);

		// ========= 지윤 =========
		EnableWindow(g_hBtnPenColor, FALSE);
		EnableWindow(g_hLineWidth, FALSE);
		ShowWindow(g_hDrawingTextId, SW_HIDE);
		ShowWindow(g_hDrawingText, SW_HIDE);

		AddLineWidthOption(hDlg);

		// ========= 정호 =========
		AddFigureOption(hDlg);
		EnableWindow(g_hFigureSelect, FALSE);
		//

		


		// ========= 
		// 컨트롤 상태 얻기
		GetDlgItemTextA(hDlg, IDC_IPADDR, g_ipaddr, sizeof(g_ipaddr));
		g_port = GetDlgItemInt(hDlg, IDC_PORT, NULL, TRUE);
		//g_isIPv6 = SendMessage(hChkIsIPv6, BM_GETCHECK, 0, 0);
		//g_isUDP = SendMessage(hChkIsUDP, BM_GETCHECK, 0, 0);

		//=============  지안 ===============//
		// 채널에 따라 UDP, TCP 체크 여부 바꾸기
		switch (channel) {
		case CHANNEL_TCP: //TCP면 UDP 채널 falsem
			g_isUDP = false;
			break;
		case CHANNEL_UDP1: //UDP면 udp 버튼 true
			g_isUDP = true;
			break;
		case CHANNEL_UDP2: //UDP면 udp 버튼 true
			g_isUDP = true;
		default:
			break;
		}
		//=====================================//

		// 소켓 통신 스레드 시작
		g_hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
		if (g_hClientThread == NULL) exit(0);
		// 서버 접속 성공 기다림
		while (g_bCommStarted == false);

		EnableWindow(hBtnGameStart, TRUE);
		WaitForSingleObject(g_hReadEvent, INFINITE);
		// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
		g_chatmsg.type = TYPE_NOTY;
		sprintf(g_chatmsg.msg, "[%s] 님이 입장하였습니다!", NICKNAME_CHAR);
		SetEvent(g_hWriteEvent);

		// 윈도우 클래스 등록
		WNDCLASS wndclass;
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = ChildWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = g_hInstance;
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = _T("MyWndClass");
		wndclass.lpszClassName = _T("MyWndClass");
		if (!RegisterClass(&wndclass)) exit(1);

		// 자식 윈도우 생성
		RECT rect; GetWindowRect(hStaticDummy, &rect);
		POINT pt; pt.x = rect.left; pt.y = rect.top;
		ScreenToClient(hDlg, &pt);
		g_hDrawWnd = CreateWindow(_T("MyWndClass"), _T(""), WS_CHILD,
			pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top,
			hDlg, (HMENU)NULL, g_hInstance, NULL);
		if (g_hDrawWnd == NULL) exit(1);
		ShowWindow(g_hDrawWnd, SW_SHOW);
		UpdateWindow(g_hDrawWnd);
		// 컨트롤 상태 변경
		EnableWindow(hChkIsIPv6, FALSE);
		EnableWindow(hEditIPaddr, FALSE);
		EnableWindow(hEditPort, FALSE);
		EnableWindow(hChkIsUDP, FALSE);
		EnableWindow(hBtnConnect, FALSE);
		EnableWindow(g_hBtnSendFile, TRUE);
		EnableWindow(g_hBtnSendMsg, TRUE);
		SetFocus(hEditMsg);
		EnableWindow(g_hBtnErasePic, TRUE);

		// ========= 지윤 =========
		EnableWindow(g_hBtnPenColor, TRUE);
		EnableWindow(g_hLineWidth, TRUE);
		ShowWindow(g_hDrawingTextId, SW_SHOW);
		ShowWindow(g_hDrawingText, SW_SHOW);


		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_GAMESTART:

			// ========= 연경 =========
			EnableWindow(hBtnGameStart, TRUE);

			//WaitForSingleObject(g_hReadEvent, INFINITE);
			//SetEvent(g_hWriteEvent);
			//isMessageQueue = TRUE;
			// 이전에 얻은 채팅 메시지 읽기 완료를 기다림

			EnableWindow(hBtnGameStart, FALSE);
			// 이전에 얻은 채팅 메시지 읽기 완료를 기다림
			WaitForSingleObject(g_hReadEvent, INFINITE);
			// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
			g_chatmsg.type = TYPE_NOTY;
			strcpy(g_chatmsg.msg, "게임이 시작됩니다!");
			SetEvent(g_hWriteEvent);
			//	gameStart(g_hTimerStatus, g_hWordStatus);
			// ========= 정호 =========
			EnableWindow(g_hFigureSelect, TRUE);
			//

			// ========= 지윤 =========

			DisplayDrawingUserID(hDlg, userIDs);

			return TRUE;
		case IDC_SENDFILE:
			MessageBox(NULL, _T("아직 구현하지 않았습니다."), _T("알림"), MB_ICONERROR);
			// 1. GetOpenFileName() 윈도우 API를 이용해서 파일 열기 대화상자를 열고
			// 전송할 파일을 선택한다. 2. 선택한 파일을 읽어서 서버에 전송한다.
			return TRUE;
		case IDC_SENDMSG:
			g_chatmsg.type = TYPE_CHAT;
			// 이전에 얻은 채팅 메시지 읽기 완료를 기다림
			WaitForSingleObject(g_hReadEvent, INFINITE);
			// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
			GetDlgItemTextA(hDlg, IDC_MSG, g_chatmsg.msg, SIZE_DAT);
			SetEvent(g_hWriteEvent);
			// 입력된 텍스트 전체를 선택 표시
			SendMessage(hEditMsg, EM_SETSEL, 0, -1);
			return TRUE;
		case IDC_ERASEPIC:
			send(g_sock, (char*)&g_erasepicmsg, SIZE_TOT, 0);
			return TRUE;
		//	======== 지윤 ==========
		case IDC_PENCOLOR:
			SelectPenColor(&g_clientDrawDetailInformation);
			return TRUE;
		case IDC_LINEWIDTH:
			SelectLineWidth(hDlg, &g_clientDrawDetailInformation);
			return TRUE;

		case IDC_EXIT_BUTTON:
			// 이전에 얻은 채팅 메시지 읽기 완료를 기다림
			WaitForSingleObject(g_hReadEvent, INFINITE);
			// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
			snprintf(g_chatmsg.msg, sizeof(g_chatmsg), "[%s]님이 퇴장하였습니다.", NICKNAME_CHAR);
			SetEvent(g_hWriteEvent);

			closesocket(g_sock);
			EndDialog(hDlg, 0);

			//CreateRankDlg(hDlg);
			return TRUE;
		// ========= 정호 ===========
		case IDC_FIGURE:
			SelectFigureOption(hDlg, g_currentSelectFigureMode);
			// "지우개" 모드에서는 색상 선택 불가능으로 설정
			if (g_currentSelectFigureMode == MODE_ERASE)
			{
				EnableWindow(g_hBtnPenColor, FALSE);
				// 색상 흰색 고정
				if (!g_isBeforeModeErase)
				{
					g_isBeforeModeErase = true;
					g_lastSelectColor = g_clientDrawDetailInformation.color;
					g_clientDrawDetailInformation.color = RGB(255, 255, 255);
				}
			}
			else
			{
				if (g_isBeforeModeErase)
				{	
					g_isBeforeModeErase = false;
					g_clientDrawDetailInformation.color = g_lastSelectColor;
					EnableWindow(g_hBtnPenColor, TRUE);
				}
			}
			return TRUE;
		//
		case IDCANCEL:
			// 이전에 얻은 채팅 메시지 읽기 완료를 기다림
			WaitForSingleObject(g_hReadEvent, INFINITE);
			// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
			snprintf(g_chatmsg.msg, sizeof(g_chatmsg), "[%s]님이 퇴장하였습니다.", NICKNAME_CHAR);
			SetEvent(g_hWriteEvent);

			closesocket(g_sock);
			EndDialog(hDlg, 0);
			return TRUE;

		//case IDC_GAMESTART:
		//	EnableWindow(hBtnGameStart, FALSE);
		//	// 이전에 얻은 채팅 메시지 읽기 완료를 기다림
		//	WaitForSingleObject(g_hReadEvent, INFINITE);
		//	// 새로운 채팅 메시지를 얻고 쓰기 완료를 알림
		//	g_chatmsg.type = TYPE_NOTY;
		//	strcpy(g_chatmsg.msg, "게임이 시작됩니다!");
		//	SetEvent(g_hWriteEvent);
		////	gameStart(g_hTimerStatus, g_hWordStatus);
		//	break;
		}
	}
	return FALSE;
}

// 자식 윈도우 프로시저(그림판 영역)
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	HPEN hPen, hOldPen;
	PAINTSTRUCT ps;
	static int cx, cy;
	static HBITMAP hBitmap;
	static HDC hDCMem;
	static int x0, y0;
	static int x1, y1;
	static bool bDrawing;

	switch (uMsg) {
	case WM_SIZE:
		// 화면 출력용 DC 핸들 얻기
		hDC = GetDC(hWnd);
		// 배경 비트맵과 메모리 DC 생성
		cx = LOWORD(lParam);
		cy = HIWORD(lParam);
		hBitmap = CreateCompatibleBitmap(hDC, cx, cy);
		hDCMem = CreateCompatibleDC(hDC);
		SelectObject(hDCMem, hBitmap);
		// 배경 비트맵 흰색으로 채움
		SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));
		SelectObject(hDCMem, GetStockObject(WHITE_PEN));
		Rectangle(hDCMem, 0, 0, cx, cy);
		// 화면 출력용 DC 핸들 해제
		ReleaseDC(hWnd, hDC);
		return 0;
	case WM_PAINT:
		// 화면 출력용 DC 핸들 얻기
		hDC = BeginPaint(hWnd, &ps);
		// 배경 비트맵을 화면에 전송
		BitBlt(hDC, 0, 0, cx, cy, hDCMem, 0, 0, SRCCOPY);
		// 화면 출력용 DC 핸들 해제
		EndPaint(hWnd, &ps);
		return 0;

	case WM_LBUTTONDOWN:
		// 마우스 클릭 좌표 얻기
		x0 = LOWORD(lParam);
		y0 = HIWORD(lParam);
		bDrawing = true;
		return 0;
		// ======= 정호 =======
	case WM_MOUSEMOVE:
		if (bDrawing && g_bCommStarted) {
			if (g_currentSelectFigureMode == MODE_ERASE || g_currentSelectFigureMode == MODE_LINE)
			{
				// 마우스 클릭 좌표 얻기
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				// 선 그리기 메시지 보내기
				g_drawlinemsg.x0 = x0;
				g_drawlinemsg.y0 = y0;
				g_drawlinemsg.x1 = x1;
				g_drawlinemsg.y1 = y1;

				// 전송 메시지의 선에 대한 색상과 굵기 설정
				g_drawlinemsg.color = g_clientDrawDetailInformation.color;
				g_drawlinemsg.width = g_clientDrawDetailInformation.width;

				//sendMsgLen(g_sock, sizeof(g_drawlinemsg));
				//sendn(g_sock, (char*)&g_drawlinemsg, sizeof(g_drawlinemsg), 0);
				sendn(g_sock, (char*)&g_drawlinemsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				// 마우스 클릭 좌표 갱신
				x0 = x1;
				y0 = y1;
			}
		}
		return 0;
	case WM_LBUTTONUP:
		// 서버와 연결이 완료되었을 때
		if (g_bCommStarted)
		{
			switch (g_currentSelectFigureMode)
			{
			// "지우개" 모드
			case MODE_ERASE:
				break;
			// "타원" 그리기 모드
			case MODE_ELLIPSE:
				g_drawellipsemsg.x0 = x0;
				g_drawellipsemsg.y0 = y0;
				g_drawellipsemsg.x1 = LOWORD(lParam);
				g_drawellipsemsg.y1 = HIWORD(lParam);
				g_drawellipsemsg.color = g_clientDrawDetailInformation.color;
				g_drawellipsemsg.width = g_clientDrawDetailInformation.width;
				//sendMsgLen(g_sock, sizeof(g_drawellipsemsg));
				//sendn(g_sock, (char*)&g_drawellipsemsg, sizeof(g_drawellipsemsg), 0);
				sendn(g_sock, (char*)&g_drawellipsemsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			// "사각형" 그리기 모드
			case MODE_RECTANGLE:
				g_drawpolygonmsg.type = TYPE_DRAWRECTANGLE;
				g_drawpolygonmsg.startX = x0;
				g_drawpolygonmsg.startY = y0;
				g_drawpolygonmsg.endX = LOWORD(lParam);
				g_drawpolygonmsg.endY = HIWORD(lParam);
				g_drawpolygonmsg.color = g_clientDrawDetailInformation.color;
				g_drawpolygonmsg.width = g_clientDrawDetailInformation.width;
				sendn(g_sock, (char*)&g_drawpolygonmsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			// "삼각형" 그리기 모드
			case MODE_TRIANGLE:
				g_drawpolygonmsg.type = TYPE_DRAWTRIANGLE;
				g_drawpolygonmsg.startX = x0;
				g_drawpolygonmsg.startY = y0;
				g_drawpolygonmsg.endX = LOWORD(lParam);
				g_drawpolygonmsg.endY = HIWORD(lParam);
				g_drawpolygonmsg.color = g_clientDrawDetailInformation.color;
				g_drawpolygonmsg.width = g_clientDrawDetailInformation.width;
				sendn(g_sock, (char*)&g_drawpolygonmsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			// "직선" 그리기 모드
			case MODE_STRAIGHT:
				g_drawpolygonmsg.type = TYPE_DRAWSTRAIGHT;
				g_drawpolygonmsg.startX = x0;
				g_drawpolygonmsg.startY = y0;
				g_drawpolygonmsg.endX = LOWORD(lParam);
				g_drawpolygonmsg.endY = HIWORD(lParam);
				g_drawpolygonmsg.color = g_clientDrawDetailInformation.color;
				g_drawpolygonmsg.width = g_clientDrawDetailInformation.width;
				sendn(g_sock, (char*)&g_drawpolygonmsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			// "오각형" 그리기 모드
			case MODE_PENTAGON:
				g_drawpolygonmsg.type = TYPE_DRAWPENTAGON;
				g_drawpolygonmsg.startX = x0;
				g_drawpolygonmsg.startY = y0;
				g_drawpolygonmsg.endX = LOWORD(lParam);
				g_drawpolygonmsg.endY = HIWORD(lParam);
				g_drawpolygonmsg.color = g_clientDrawDetailInformation.color;
				g_drawpolygonmsg.width = g_clientDrawDetailInformation.width;
				sendn(g_sock, (char*)&g_drawpolygonmsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			// "별" 그리기 모드
			case MODE_STAR:
				g_drawpolygonmsg.type = TYPE_DRAWSTAR;
				g_drawpolygonmsg.startX = x0;
				g_drawpolygonmsg.startY = y0;
				g_drawpolygonmsg.endX = LOWORD(lParam);
				g_drawpolygonmsg.endY = HIWORD(lParam);
				g_drawpolygonmsg.color = g_clientDrawDetailInformation.color;
				g_drawpolygonmsg.width = g_clientDrawDetailInformation.width;
				sendn(g_sock, (char*)&g_drawpolygonmsg, SIZE_TOT, 0, serveraddr, g_isUDP);
				break;

			default:
				break;
			}
		}
		bDrawing = false;
		return 0;
	// ======== 정호 ==========
	// 선 그리기 메시지 받음
	case WM_DRAWLINE:
		DrawLineProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation);
		return 0;

	// 타원 그리기 메시지 받음
	case WM_DRAWELLIPSE:
		DrawEllipseProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation);
		return 0;

	// 특정 부분 조금 지우기 윈도우 메시지 받음
	case WM_ERASEALITTLE:
		DrawLineProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation);
		return 0;

	// 사각형 그리기 메시지 받음
	case WM_DRAWRECTANGLE:
		DrawPolygonProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation, MODE_RECTANGLE);

		return 0;

	// 삼각형 그리기 메시지 받음
	case WM_DRAWTRIAGNGLE:
		DrawPolygonProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation, MODE_TRIANGLE);
		return 0;
	
	// 직선 그리기 메시지 받음
	case WM_DRAWSTRAIGHT:
		DrawPolygonProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation, MODE_STRAIGHT);
		return 0;

	// 오각형 그리기 메시지 받음
	case WM_DRAWPENTAGON:
		DrawPolygonProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation, MODE_PENTAGON);
		return 0;

	// 별 그리기 메시지 받음
	case WM_DRAWSTAR:
		DrawPolygonProcess(hWnd, hDCMem, wParam, lParam, g_serverDrawDetailInformation, MODE_STAR);
		return 0;
		
	//
	case WM_ERASEPIC:
		// 배경 비트맵 흰색으로 채움
		SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));
		SelectObject(hDCMem, GetStockObject(WHITE_PEN));
		Rectangle(hDCMem, 0, 0, cx, cy);
		// WM_PAINT 메시지 강제 생성
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_DESTROY:
		DeleteDC(hDCMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//// ---- 지안 변수 (로그인을 위함) ----- //
//_TCHAR input_result[256]; // input 결과 저장할 배열
//_TCHAR ID_NICKNAME[256]; // stdafx.h 파일에 같은 주소에 저장하기 위함
//
//// 홈 창 변수
//int channel;	//udp 채널 가져오기. stdafx.h 파일에 같은 주소에 저장하기 위함
//
////-------------------------------//

// ---- 연경 ------------------- //


//-------------------------------//

// 로그인 윈도우 프로시저 (로그인 영역) -----------------------------------------------------------------------------------//
LRESULT CALLBACK LoginWndProc(HWND hwndLogin, UINT msg, WPARAM wParam, LPARAM lParam) {
	g_isDup == 0; //중복확인 비활성화
	switch (msg) {

	case WM_CREATE:
		// 로그인 화면 초기화 및 컨트롤 생성
																								   //x,y,width,height
		CreateWindow(_T("STATIC"), _T("스케치퀴즈"), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 500, 100, 300, 100, hwndLogin, NULL, NULL, NULL); // 스케치퀴즈 타이틀
		CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 400, 300, 400, 40, hwndLogin, (HMENU)ID_ID_INPUT, NULL, NULL);
		CreateWindow(_T("BUTTON"), _T("중복확인"), WS_VISIBLE | WS_CHILD, 850, 300, 100, 40, hwndLogin, (HMENU)ID_DUPLICATION_BUTTON, NULL, NULL); // 중복확인 버튼
		//CreateWindow(_T("STATIC"), _T("사용 가능한 ID입니다!"), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 400, 360, 400, 40, hwndLogin, NULL, NULL, NULL); // 사용 가능한 ID입니다!
		CreateWindow(_T("BUTTON"), _T("로그인"), WS_VISIBLE | WS_CHILD | WS_DISABLED, 400, 500, 500, 100, hwndLogin, (HMENU)ID_LOGIN_BUTTON, NULL, NULL); // 로그인 버튼 (처음 비활성화)
		CreateWindow(_T("BUTTON"), _T("돌아가기"), WS_VISIBLE | WS_CHILD, 100, 100, 100, 30, hwndLogin, (HMENU)ID_BACKHOME_BUTTON, NULL, NULL);
		break;

	case WM_COMMAND:
		// 버튼 클릭 이벤트 처리
		switch (LOWORD(wParam)) {
			// '돌아가기' 버튼 클릭 처리
		case ID_BACKHOME_BUTTON:
			// 로그인 창을 숨기고 메인창을 보이게 해야하는데 일단 로그인창 숨김
			ShowWindow(hwndLogin, SW_HIDE);
			break;

		case ID_LOGIN_BUTTON: // 로그인 버튼을 클릭했을 시
			
			_tcscpy(ID_NICKNAME, input_result); // 현재 입력한 ID 저장
			MessageBox(hwndLogin, ID_NICKNAME, _T("메인 화면으로 이동합니다."), MB_OK);

			// ==================== 지윤 ====================
			AddUser(userIDs, input_result);
			// ==============================================

			CreateAndShowWindow_Home(hwndHome); // 메인 생성 및 보이게하기
			ShowWindow(hwndLogin, SW_HIDE);
			break;

		case ID_ID_INPUT: // ID를 입력해주세요! 입력했을 시
			break;

		case ID_DUPLICATION_BUTTON: //중복 확인 버튼 클릭했을 시, 중복 확인하기
			_TCHAR userId[256]; // 이미 있는 유저아이디?
			_tcscpy(userId, _T("abc123")); // Copy the string "abc123" into userId
			GetDlgItemText(hwndLogin, ID_ID_INPUT, input_result, sizeof(input_result));


			// ---- 로그인 할때 TCP 연결 ---- //
			_tcscpy(ID_NICKNAME, input_result); // 현재 입력한 ID 저장
			WideCharToMultiByte(CP_ACP, 0, ID_NICKNAME, 256, NICKNAME_CHAR, 256, NULL, NULL); //_TCHAR 형 문자열을 char* 형 문자열로 변경
			// 스레드 생성!
			//LoginProcessClient(); //TCP 연결. ->
			// 스레드 생성
			LoginProcessClientThread = CreateThread(NULL, 0, LoginProcessClient, NULL, 0, NULL);

			//// 스레드 종료 대기
			//WaitForSingleObject(LoginProcessClientThread, INFINITE);

			//// 스레드 핸들 닫기
			//CloseHandle(LoginProcessClientThread);
			// ---------------------------- //
			 
			Sleep(1000);
			MessageBox(hwndLogin, recvBuf_tchar, _T("서버 중복 확인 결과"), MB_OK);
		// 현재 있는 Id와, 입력한 아이디 와의 비교
			if (_tcscmp(recvBuf_tchar, _T("false")) == 0)
			{
				MessageBox(hwndLogin, _T("이미 있는 아이디입니다. 다른 아이디를 사용해주세요."), _T("중복 확인 결과"), MB_OK);

				// 이미 있는 아이디인 경우 로그인 버튼 비활성화
				EnableWindow(GetDlgItem(hwndLogin, ID_LOGIN_BUTTON), FALSE);
			}
			else
			{
				MessageBox(hwndLogin, _T("사용 가능한 아이디입니다."), _T("중복 확인 결과"), MB_OK);

				// 사용 가능한 아이디인 경우 로그인 버튼 활성화
				EnableWindow(GetDlgItem(hwndLogin, ID_LOGIN_BUTTON), TRUE);
			}
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwndLogin, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------//


//-------------------------------------홈 윈도우 프로시저 -----------------------------------------------------------------------//
// 윈도우 프로시저 (지안)
LRESULT CALLBACK HomeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND hAChannelDlg = NULL;
	static HWND hBChannelDlg = NULL;
	switch (msg) {

	case WM_CREATE:
		// 홈 메인 화면 초기화 및 컨트롤 생성
																							   //x,y,width,height
		CreateWindow(_T("STATIC"), ID_NICKNAME, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 900, 10, 100, 30, hwnd, NULL, NULL, NULL); // 유저 id 출력
		CreateWindow(_T("STATIC"), _T("님 반갑습니다!"), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 1000, 10, 200, 30, hwnd, NULL, NULL, NULL); // id 님 반갑습니다!
		CreateWindow(_T("STATIC"), _T("~ 스케치 퀴즈 ~"), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 50, 50, 1150, 100, hwnd, NULL, NULL, NULL); // 스케치퀴즈 타이틀

		CreateWindow(_T("BUTTON"), _T("공지 전송"), WS_VISIBLE | WS_CHILD, 1042, 185, 174, 54, hwnd, (HMENU)ID_NOTICE_BUTTON, NULL, NULL); // 공지 전송

		CreateWindow(_T("BUTTON"), _T("TCP 채널 	"), WS_VISIBLE | WS_CHILD, 300, 200, 640, 100, hwnd, (HMENU)ID_CHANNEL_A_BUTTON, NULL, NULL); // 채널 A 입장
		CreateWindow(_T("BUTTON"), _T("UDP 채널1 입장"), WS_VISIBLE | WS_CHILD, 300, 350, 640, 100, hwnd, (HMENU)ID_CHANNEL_B_BUTTON, NULL, NULL); // 채널 B 입장

		//CreateWindow(L"BUTTON", L"방만들기", WS_VISIBLE | WS_CHILD, 282, 600, 320, 67, hwnd, (HMENU)ID_BACKHOME_BUTTON, NULL, NULL); // 방 만들기
		CreateWindow(_T("BUTTON"), _T("UDP 채널2 입장"), WS_VISIBLE | WS_CHILD, 300, 500, 640, 100, hwnd, (HMENU)ID_CHANNEL_RANDOM_BUTTON, NULL, NULL); // 랜덤 입장


		CreateWindow(_T("BUTTON"), _T("돌아가기"), WS_VISIBLE | WS_CHILD, 100, 100, 100, 30, hwnd, (HMENU)ID_BACKHOME_BUTTON, NULL, NULL); // 돌아가기
		break;
		//----//
		//CreateWindow(_T("BUTTON"), _T("UDP 채널1 입장"), WS_VISIBLE | WS_CHILD, 300, 250, 640, 150, hwnd, (HMENU)ID_CHANNEL_B_BUTTON, NULL, NULL); // 채널 B 입장
		//CreateWindow(_T("BUTTON"), _T("UDP 채널2 입장"), WS_VISIBLE | WS_CHILD, 300, 450, 640, 150, hwnd, (HMENU)ID_CHANNEL_RANDOM_BUTTON, NULL, NULL); // 랜덤 입장
		//----//

	case WM_COMMAND:
		// 버튼 클릭 이벤트 처리
		switch (LOWORD(wParam)) {
			// '돌아가기' 버튼 클릭 처리
		case ID_BACKHOME_BUTTON:
			// 로그인 창을 숨기고 메인 창을 다시 보이게 함
		/*	ShowWindow(hwnd, SW_SHOW);*/
			ShowWindow(hwnd, SW_HIDE);
			break;

		case ID_NOTICE_BUTTON: //[공지 전송] 버튼 클릭시
			CreateAndShowWindow_Home_Pass(hwndHome_Pass); // 관리자 비밀번호 입력 창 띄우기
			break;

		case ID_CHANNEL_A_BUTTON: // TCP 채널 버튼 클릭시
			channel = CHANNEL_TCP;	// tcp 채널 버전 0으로 변경
			CreateAndShowDialog(hwnd);
			break;
		case ID_CHANNEL_B_BUTTON: // UDP 채널1 버튼 클릭시
			channel = CHANNEL_UDP1;	//udp 채널 버전 1로 변경
			CreateAndShowDialog(hwnd);
			break;
		case ID_CHANNEL_RANDOM_BUTTON: // UDP 채널2 버튼 클릭시
			channel = CHANNEL_UDP2;	//udp 채널 버전 2로 변경
			CreateAndShowDialog(hwnd);
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------//


//------------------------------------- 홈 공지사항 윈도우 프로시저 -----------------------------------------------------------------------//
// 
 _TCHAR input_notice_result[256]; // input 결과 저장할 배열
// 
// 홈 공지사항 윈도우 프로시저
LRESULT CALLBACK Home_NoticeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_CREATE:
		// 로그인 화면 초기화 및 컨트롤 생성
																								   //x,y,width,height
		CreateWindow(_T("STATIC"), _T("공지사항 입력"), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 200, 50, 200, 50, hwnd, NULL, NULL, NULL); // 스케치퀴즈 타이틀
		CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 150, 400, 50, hwnd, (HMENU)ID_NOTICE_INPUT, NULL, NULL);	// 공지사항 입력 input
		CreateWindow(_T("BUTTON"), _T("확인"), WS_VISIBLE | WS_CHILD, 180, 300, 120, 50, hwnd, (HMENU)ID_OK_BUTTON, NULL, NULL); // 확인 버튼
		CreateWindow(_T("BUTTON"), _T("취소"), WS_VISIBLE | WS_CHILD, 320, 300, 120, 50, hwnd, (HMENU)ID_CANCLE_BUTTON, NULL, NULL);		// 취소 버튼
		break;

	case WM_COMMAND:
		// 버튼 클릭 이벤트 처리
		switch (LOWORD(wParam)) {
			// '취소' 버튼 클릭 처리
		case ID_CANCLE_BUTTON:
			// 홈 공지사항 입력창을 숨기고 메인 창을 다시 보이게 함
		/*	ShowWindow(hwnd, SW_SHOW);*/
			ShowWindow(hwnd, SW_HIDE);
			break;
		case ID_OK_BUTTON: //확인 버튼 클릭시 - input 내용이 저장됨.
			

			GetDlgItemText(hwnd, ID_NOTICE_INPUT, input_notice_result, sizeof(input_notice_result));
			MessageBox(hwnd, input_notice_result, _T("공지사항 전송 내용"), MB_OK); // 공지사항 전송할 내용 띄우기
			BoradcaseSendToNotice(input_notice_result); //전송가느자

			//ShowWindow(hwnd, SW_HIDE);	// 공지사항 입력창 닫기
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------//


//------------------------------------- 홈 공지사항 비밀번호 윈도우 프로시저 -----------------------------------------------------------------------//
// 홈 공지사항 윈도우 프로시저
LRESULT CALLBACK Home_PassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_CREATE:
		// 로그인 화면 초기화 및 컨트롤 생성
																								   //x,y,width,height
		CreateWindow(_T("STATIC"), _T("관리자 비밀번호를 입력해주세요."), WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE, 100, 50, 400, 50, hwnd, NULL, NULL, NULL); // 관리자 비밀번호 타이틀
		CreateWindow(_T("EDIT"), _T("비밀번호 입력하기"), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 150, 400, 50, hwnd, (HMENU)ID_PASSWORD_INPUT, NULL, NULL);	// 비밀번호 입력 input
		CreateWindow(_T("BUTTON"), _T("확인"), WS_VISIBLE | WS_CHILD, 180, 300, 120, 50, hwnd, (HMENU)ID_OK_BUTTON, NULL, NULL); // 확인 버튼
		CreateWindow(_T("BUTTON"), _T("취소"), WS_VISIBLE | WS_CHILD, 320, 300, 120, 50, hwnd, (HMENU)ID_CANCLE_BUTTON, NULL, NULL);		// 취소 버튼
		break;

	case WM_COMMAND:
		// 버튼 클릭 이벤트 처리
		switch (LOWORD(wParam)) {
			// '취소' 버튼 클릭 처리
		case ID_CANCLE_BUTTON:
			// 홈 공지사항 비밀번호 입력창을 숨기고 메인 창을 다시 보이게 함
		/*	ShowWindow(hwnd, SW_SHOW);*/
			ShowWindow(hwnd, SW_HIDE);
			break;
		case ID_OK_BUTTON: //확인 버튼 클릭시 - input 내용이 저장됨.
			_TCHAR password[256]; // 이미 있는 유저아이디?
			_tcscpy(password, _T("abc123")); // Copy the string "abc123" into userId
			
			GetDlgItemText(hwnd, ID_PASSWORD_INPUT, input_result, sizeof(input_result));


			// 비밀번호가 일치하는지 비교
			if (_tcscmp(password, input_result) == 0 )
			{
				CreateAndShowWindow_Home_Notice(hwndHome_Notice); // 공지사항 입력 화면 보여주기
				ShowWindow(hwnd, SW_HIDE);
			}
			else
			{
				MessageBox(hwndHome_Pass, _T("비밀번호가 일치하지 않습니다. 다시 입력해주세요."), _T("오류"), MB_OK);
				
			}
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------//

//--------------------------------------------------지안----------------------------------------------------------------//
// 소켓 통신 스레드 함수 (0) - 로그인할때 소켓 통신하기
// 클라이언트와 데이터 통신
DWORD WINAPI LoginProcessClient(LPVOID arg)
{
	// ====== 지안 ========= //
	// id_msg 구조체 초기화
	ID_MSG id_msg;
	id_msg.type = TYPE_ID;	//id타입
	strcpy(id_msg.msg, NICKNAME_CHAR);	//NICKNAME_CHAR일 경우

	// ===================== //
	int retval;
	// socket()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) err_quit("socket()");
	
	// connect() : 기존 TCP 연결이 아닌 새로 만든것
	SOCKADDR_IN Loginserveraddr;
	ZeroMemory(&Loginserveraddr, sizeof(Loginserveraddr));

	Loginserveraddr.sin_family = AF_INET; //server에 대한 설정
	Loginserveraddr.sin_addr.s_addr = inet_addr(SERVERIP4_CHAR); //server에 대한 설정
	Loginserveraddr.sin_port = htons(SERVERPORT); //server에 대한 설정
	retval = connect(g_sock, (SOCKADDR*)&Loginserveraddr, sizeof(Loginserveraddr)); //연결할때, 서버소켓 정보를 준다. -> establishied 상태
	if (retval == SOCKET_ERROR) err_quit("connect()");

	int len;
	len = sizeof(NICKNAME_CHAR);

	// 고정 크기 데이터 전송 (TCP 첫 실행시 한번)
	retval = sendn(g_sock, (char*)&id_msg, BUFSIZE, 0, serveraddr, false);

	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		//break;
	}

	
	//char recvBuf[BUFSIZE]; // 데이터 받을 버퍼
	//_TCHAR recvBuf_tchar[BUFSIZE]; // tchar로 받을 버퍼
	while (1) {

		// 데이터 받기
		retval = recv(g_sock, recvBuf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return 0;
		}
		//else if (retval == 0)
		//	break;
		// 받은 데이터 출력
		recvBuf[retval] = '\0';
		//printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		//printf("[받은 데이터] %s\n", recvBuf);
		MultiByteToWideChar(CP_ACP, 0, recvBuf, -1, recvBuf_tchar, BUFSIZE); // char* 형 문자열을 _TCHAR 형 문자열로 변환
		//MessageBox(NULL, recvBuf_tchar, _T("TCP 데이터를 받았어요"), MB_ICONERROR);

	}
	

	if (retval == SOCKET_ERROR)
		return 0;

	return 0;


}


//----------------------------------------------------------------------------------------------------------------------//


// 소켓 통신 스레드 함수(1) - 메인
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	if (g_isIPv6 == false && g_isUDP == false) { // TCP/IPv4 서버
		// socket()
		g_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (g_sock == INVALID_SOCKET) err_quit("socket()");

		// connect()
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		inet_pton(AF_INET, g_ipaddr, &serveraddr.sin_addr);
		serveraddr.sin_port = htons(g_port);
		retval = connect(g_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");
	}
	else if (g_isIPv6 == true && g_isUDP == false) { // TCP/IPv6 서버
		// socket()
		g_sock = socket(AF_INET6, SOCK_STREAM, 0);
		if (g_sock == INVALID_SOCKET) err_quit("socket()");

		// connect()
		struct sockaddr_in6 serveraddr;
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin6_family = AF_INET6;
		inet_pton(AF_INET6, g_ipaddr, &serveraddr.sin6_addr);
		serveraddr.sin6_port = htons(g_port);
		retval = connect(g_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");
	}
	else if (g_isIPv6 == false && g_isUDP == true) { // UDP/IPv4 서버
		//========================================== 지안 ==========================================//
		
		//--------------------- UDP 서버 1 ----------------------//
		if (channel == CHANNEL_UDP1) { //UDP 채널 1 이라면
			//MessageBox(NULL, _T("지안이가 구현중인 UDP 채널1 IPv4 클라이언트 소켓임"), _T("알림"), MB_ICONERROR);
			// socket()
			g_sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (g_sock == INVALID_SOCKET) err_quit("socket()");

			// 멀티캐스트 그룹 가입 - (UDP는 연결설정을 하지 않으므로, connet() 불필요)
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(SERVERIP4_CHAR_UDP1); // 가입하거나 탈퇴할 IPv4 멀티케스트 address(주소) (가입할 동아리)
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);		// 로컬 ip address (나)
			retval = setsockopt(g_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR) err_quit("setsockopt()");

			// 소켓 주소 구조체 초기화
			ZeroMemory(&serveraddr, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(SERVERIP4_CHAR_UDP1);
			serveraddr.sin_port = htons(SERVERPORT);


			// 데이터 통신에 사용할 변수
			char buf[BUFSIZE + 1] = "hello, I'am UDP JIAN. UDP Channel1 !!";
			int len;

			// 데이터 보내기
			retval = sendto(g_sock, buf, BUFSIZE, 0,
				(SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if (retval == SOCKET_ERROR) {
				err_display("sendto()");
			}

			// 멀티캐스트 그룹 탈퇴 (연경아 udp 채널 x할때 이렇게 하셈)
			retval = setsockopt(g_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
				(char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR) err_quit("setsockopt()");
		}
		//------------------------------------------------------//

		//--------------------- UDP 서버 2 ----------------------//
		else if (channel == CHANNEL_UDP2) { //UDP 채널 2라면
			//MessageBox(NULL, _T("지안이가 구현중인 UDP 채널2 IPv4 클라이언트 소켓임"), _T("알림"), MB_ICONERROR);
			// socket()
			g_sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (g_sock == INVALID_SOCKET) err_quit("socket()");

			// 멀티캐스트 그룹 가입 - (UDP는 연결설정을 하지 않으므로, connet() 불필요)
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(SERVERIP4_CHAR_UDP2); // 가입하거나 탈퇴할 IPv4 멀티케스트 address(주소) (가입할 동아리)
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);		// 로컬 ip address (나)
			retval = setsockopt(g_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR) err_quit("setsockopt()");

			// 소켓 주소 구조체 초기화
			ZeroMemory(&serveraddr, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(SERVERIP4_CHAR_UDP2);
			serveraddr.sin_port = htons(SERVERPORT);

			// 데이터 통신에 사용할 변수
			char buf[BUFSIZE + 1] = "hello, I'am UDP JIAN. UDP Channel2 !!";
			int len;

			// 데이터 보내기
			retval = sendto(g_sock, buf, strlen(buf), 0,
				(SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if (retval == SOCKET_ERROR) {
				err_display("sendto()");
			}

			// 멀티캐스트 그룹 탈퇴 (연경아 udp 채널 x할때 이렇게 하셈)
			retval = setsockopt(g_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
				(char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR) err_quit("setsockopt()");

		}
		//------------------------------------------------------//
		
		//==========================================================================================//
	}
	else if (g_isIPv6 == true && g_isUDP == true) { // UDP/IPv6 서버
		MessageBox(NULL, _T("아직 구현하지 않았습니다."), _T("알림"), MB_ICONERROR);
		exit(1);
	}
	MessageBox(NULL, _T("서버에 접속했습니다."), _T("알림"), MB_ICONINFORMATION);

	// 읽기 & 쓰기 스레드 생성
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	if (hThread[0] == NULL || hThread[1] == NULL) exit(1);
	g_bCommStarted = true;

	// 스레드 종료 대기 (둘 중 하나라도 종료할 때까지)
	retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	retval -= WAIT_OBJECT_0;
	if (retval == 0)
		TerminateThread(hThread[1], 1);
	else
		TerminateThread(hThread[0], 1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	MessageBox(NULL, _T("연결이 끊겼습니다."), _T("알림"), MB_ICONERROR);
	EnableWindow(g_hBtnSendFile, FALSE);
	EnableWindow(g_hBtnSendMsg, FALSE);
	EnableWindow(g_hBtnErasePic, FALSE);
	g_bCommStarted = false;
	closesocket(g_sock);
	return 0;
}

// 소켓 통신 스레드 함수(2) - 데이터 수신
DWORD WINAPI ReadThread(LPVOID arg)
{
	int retval;
	COMM_MSG comm_msg;
	CHAT_MSG* chat_msg;
	DRAWLINE_MSG* drawline_msg;
	ERASEPIC_MSG* erasepic_msg;
	char reciever[20], sender[20], tmp[5];


	// ------ 연경 --------
	char senderName[256];
	char recieverName[256];
	char sendMsg[256];
	char word[10];

	// ====== 정호 ========
	DRAWELLIPSE_MSG* drawEllipse_msg;
	DRAWPOLYGON_MSG* drawPolygon_msg;
	int serveraddrLen;
	int len;
	//

	while (1) {

		//// 데이터 받기(고정 길이)
		//retval = recvn(g_sock, (char*)&len, sizeof(int), 0);
		//if (retval == SOCKET_ERROR) 
		//{
		//	err_display("recvn()");
		//	break;
		//}
		//else if (retval == 0)
		//{
		//	break;
		//}

		//// 데이터 받기(가변 길이)
		//retval = recvn(g_sock, (char*)&comm_msg, len, 0);
		//if (retval == 0 || retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//{
		//	break;
		//}
		// 

		//if (isMessageQueue == TRUE) {
		//	retval = recvn(g_sock, (char*)&g_msgQueue, BUFSIZE, 0, serveraddr, g_isUDP);
		//	if (retval == 0 || retval == SOCKET_ERROR) {
		//		err_display("recv()");
		//		break;
		//	}
		//	DisplayText("------\r\n");
		//	int idx = g_msgQueue.head;
		//	for (int i = 0; i < ((g_msgQueue.tail - g_msgQueue.head + BUFSIZE) % BUFSIZE); i++) {
		//		DisplayText(g_msgQueue.queue[idx]);
		//		DisplayText("\r\n");
		//		idx = (idx + 1) % BUFSIZE;
		//	}
		//	isMessageQueue = FALSE;
		//	continue;
		//}
		retval = recvn(g_sock, (char*)&comm_msg, BUFSIZE, 0, serveraddr, g_isUDP);

		if (retval == 0 || retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
		{
			break;
		}

		switch (comm_msg.type)
		{
			// ============ 연경 ==========
		case TYPE_CHAT:
			chat_msg = (CHAT_MSG*)&comm_msg;
			sscanf(chat_msg->msg, "{%[^}]%*s%s", senderName, sendMsg);

			if (strncmp(sendMsg, "/w ", 3) == 0) {
				sscanf(sendMsg, "%s %s %s", tmp, sender, reciever);
				if (strcmp(reciever, NICKNAME_CHAR) == 0) {
					MySendFile(sender, reciever, chat_msg->msg);
					DisplayText("%s\r\n", chat_msg->msg);
				}
			}
			else {
				DisplayText("%s\r\n", chat_msg->msg);
			}

			WideCharToMultiByte(CP_ACP, 0, quizWord[roundNum], 10, word, 10, NULL, NULL);
			if (strcmp(sendMsg, word) == 0) {  // 제시어를 맞춘 경우: 정답임을 출력하고 새 라운드 시작

				DisplayText("[%s] 정답입니다!\r\n", word);
				newRound();
			}

			break;
		case TYPE_NOTY:
			chat_msg = (CHAT_MSG*)&comm_msg;
			DisplayText("%s\r\n", chat_msg->msg);
			break;
		case TYPE_DRAWLINE:
			drawline_msg = (DRAWLINE_MSG*)&comm_msg;
			// ============ 지윤 ============
			g_serverDrawDetailInformation.width = drawline_msg->width;
			// ==============================
			g_serverDrawDetailInformation.color = drawline_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWLINE,
				MAKEWPARAM(drawline_msg->x0, drawline_msg->y0),
				MAKELPARAM(drawline_msg->x1, drawline_msg->y1));
			break;
			// ======== 정호 ==========
		case TYPE_DRAWELLIPSE:
			drawEllipse_msg = (DRAWELLIPSE_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawEllipse_msg->width;
			g_serverDrawDetailInformation.color = drawEllipse_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWELLIPSE,
				MAKEWPARAM(drawEllipse_msg->x0, drawEllipse_msg->y0),
				MAKELPARAM(drawEllipse_msg->x1, drawEllipse_msg->y1));
			break;

		// 사각형 그리기
		case TYPE_DRAWRECTANGLE:
			drawPolygon_msg = (DRAWPOLYGON_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawPolygon_msg->width;
			g_serverDrawDetailInformation.color = drawPolygon_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWRECTANGLE,
				MAKEWPARAM(drawPolygon_msg->startX, drawPolygon_msg->startY),
				MAKELPARAM(drawPolygon_msg->endX, drawPolygon_msg->endY));
			break;

		// 삼각형 그리기
		case TYPE_DRAWTRIANGLE:
			drawPolygon_msg = (DRAWPOLYGON_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawPolygon_msg->width;
			g_serverDrawDetailInformation.color = drawPolygon_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWTRIAGNGLE,
				MAKEWPARAM(drawPolygon_msg->startX, drawPolygon_msg->startY),
				MAKELPARAM(drawPolygon_msg->endX, drawPolygon_msg->endY));
			break;
		
		// 직선 그리기
		case TYPE_DRAWSTRAIGHT:
			drawPolygon_msg = (DRAWPOLYGON_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawPolygon_msg->width;
			g_serverDrawDetailInformation.color = drawPolygon_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWSTRAIGHT,
				MAKEWPARAM(drawPolygon_msg->startX, drawPolygon_msg->startY),
				MAKELPARAM(drawPolygon_msg->endX, drawPolygon_msg->endY));
			break;

		// 오각형 그리기
		case TYPE_DRAWPENTAGON:
			drawPolygon_msg = (DRAWPOLYGON_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawPolygon_msg->width;
			g_serverDrawDetailInformation.color = drawPolygon_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWPENTAGON,
				MAKEWPARAM(drawPolygon_msg->startX, drawPolygon_msg->startY),
				MAKELPARAM(drawPolygon_msg->endX, drawPolygon_msg->endY));
			break;

		// 별 그리기
		case TYPE_DRAWSTAR:
			drawPolygon_msg = (DRAWPOLYGON_MSG*)&comm_msg;
			g_serverDrawDetailInformation.width = drawPolygon_msg->width;
			g_serverDrawDetailInformation.color = drawPolygon_msg->color;
			SendMessage(g_hDrawWnd, WM_DRAWSTAR,
				MAKEWPARAM(drawPolygon_msg->startX, drawPolygon_msg->startY),
				MAKELPARAM(drawPolygon_msg->endX, drawPolygon_msg->endY));
			break;

		case TYPE_ERASEPIC:
			erasepic_msg = (ERASEPIC_MSG*)&comm_msg;
			SendMessage(g_hDrawWnd, WM_ERASEPIC, 0, 0);
			break;

		default:
			break;
		}
		


	}
	return 0;
}

// 소켓 통신 스레드 함수(3) - 데이터 송신
DWORD WINAPI WriteThread(LPVOID arg)
{
	int retval, len;
	char* nickName;

	// 서버와 데이터 통신
	while (1) {
		// 쓰기 완료 기다리기
		WaitForSingleObject(g_hWriteEvent, INFINITE);
		//if (isMessageQueue == TRUE) {
		//	isMessageQueue = FALSE;
		//	DisplayText("이전 대화 내용 표시\r\n");
		//	SetEvent(g_hReadEvent);
		//	continue;
		//}
		// 문자열 길이가 0이면 보내지 않음
		if (strlen(g_chatmsg.msg) == 0) {
			// [메시지 전송] 버튼 활성화
			EnableWindow(g_hBtnSendMsg, TRUE);
			// 읽기 완료 알리기
			SetEvent(g_hReadEvent);
			continue;
		}
		// ============ 정호 ===========
		// 데이터 보내기

		char sendMsg[256];
		if (g_chatmsg.type == TYPE_CHAT) {
			snprintf(sendMsg, sizeof(sendMsg), "{%s} %s", NICKNAME_CHAR, g_chatmsg.msg);
			strcpy(g_chatmsg.msg, sendMsg);

		}
		len = sizeof(g_chatmsg);

		// 고정 크기 데이터 전송
		retval = sendn(g_sock, (char*)&g_chatmsg, BUFSIZE, 0, serveraddr, g_isUDP);

		//retval = sendn(g_sock, (char*)&len, sizeof(int), 0);
		//// 가변 크기 데이터 전송
		//retval = sendn(g_sock, (char*)&g_chatmsg, len, 0);
		if (retval == SOCKET_ERROR) break;


		// [메시지 전송] 버튼 활성화
		EnableWindow(g_hBtnSendMsg, TRUE);
		// 읽기 완료 알리기
		SetEvent(g_hReadEvent);
	}
	return 0;
}

// 에디트 컨트롤 출력 함수
void DisplayText(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char cbuf[1024];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	int nLength = GetWindowTextLength(g_hEditStatus);
	SendMessage(g_hEditStatus, EM_SETSEL, nLength, nLength);
	SendMessageA(g_hEditStatus, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
}

void btnGameStart_click() {

}
