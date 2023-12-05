#include "stdafx.h"

// 홈 창을 만들고 표시하는 함수
void CreateAndShowWindow_Home_Notice(HWND hWnd)
{
	// 홈 창 생성
	hwndHome_Notice = CreateWindow(_T("Home_NoticeWindowClass"), _T("공지사항 입력창"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, g_hInstance, NULL);

	if (hwndHome_Notice == NULL)
	{
		MessageBox(NULL, L"윈도우 생성 실패!", L"에러", MB_ICONERROR);
	}
	ShowWindow(hwndHome_Notice, SW_SHOW);
}


// 공지사항을 UDP 브로드캐스트로 보내는 함수




//#define REMOTEPORT 9000

int BoradcaseSendToNotice(_TCHAR* input_notice_result)
{
	MessageBox(hwndHome_Notice, input_notice_result, _T("공지사항 전송 내용 잘갔는지 확인"), MB_OK);
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 브로드캐스팅 활성화
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr; //상대방
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(SERVERIP4_CHAR); //상대방의 ip
	remoteaddr.sin_port = htons(SERVERPORT); // 아까 설정해둔 상대방 port번호

	// 데이터 통신에 사용할 변수
	char buf[257];
	int len;

	// 브로드캐스트 데이터 보내기

	// 데이터 입력
										//바꿀 대상 - 저장할 곳
	WideCharToMultiByte(CP_ACP, 0, input_notice_result, 256, buf, 256, NULL, NULL); //_TCHAR 형 문자열을 char* 형 문자열로 변경

	// ================== //

	// notice_msg 구조체 초기화
	NOTICE_MSG notice_msg;
	notice_msg.type = TYPE_NOTICE;	// notice (공지사항) 타입
	strcpy(notice_msg.msg, buf);	// msg에 공지사항 내용을 넣는다. (char)
	// ================== //
	
	// 데이터 보내기
	retval = sendto(sock, (char*)&notice_msg, strlen(buf), 0,
		(SOCKADDR*)&remoteaddr, sizeof(remoteaddr)); //&remoteaddr : 보낼때 목적지 주소를 무조건 넣어줘야한다.
	if (retval == SOCKET_ERROR) {
		err_display("sendto()");
	}
	MessageBox(NULL, input_notice_result, _T("UDP 데이터를 보냈어요"), MB_OK);
	//printf("[UDP] %d바이트를 보냈습니다.\n", retval);


	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;

}

