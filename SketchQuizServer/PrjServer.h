#pragma once

#define SERVERIP4_CHAR_UDP1 "235.7.8.18" // 그룹 1
#define SERVERIP4_CHAR_UDP2 "235.7.8.19" // 그룹 1
#define SERVERPORT 9000
#define BUFSIZE    256

// ==== 연경 ====
#define TYPE_CHAT   1000
// =============

//==== 지안 ===== //
#define SIZE_TOT 256                    // 전송 패킷(헤더 + 데이터) 전체 크기
#define SIZE_DAT (SIZE_TOT-sizeof(int)) // 헤더를 제외한 데이터 부분만의 크기

#define TYPE_ID		1003				// 메시지 타입: id (지안)
// ============== //

// ==== 정호 ====
#define WM_SOCKET (WM_USER+1)

// 소켓 정보 저장을 위한 구조체와 변수
typedef struct _SOCKETINFO
{
	SOCKET sock;
	char   buf[BUFSIZE + 1];
	int    recvbytes;
	// ===== 지안 ====== 
	USHORT sin_port;	// 소켓 포트 번호
	IN_ADDR sin_addr;	// 소켓 ip 주소 
	char id_nickname_char[BUFSIZE]; // 사용자 닉네임 char 버전.

	// ===== 연경 ======
	_TCHAR id_nickname[BUFSIZE]; // 사용자 닉네임
	int    score = 0;       // 게임 플레이 점수
} SOCKETINFO;

// ======= 연경 ======= 
typedef struct _MESSAGEQUEUE {
	char queue[BUFSIZE][100] = { NULL };         // 메시지 원형 큐: 이전 대화내용 표시. 꽉 차면 가장 오래된 메시지부터 지워진다.
	int head = 0;                 // 원형 큐 인덱스
	int tail = 0;
} MESSAGEQUEUE;

typedef struct _CHAT_MSG
{
	int  type;
	char msg[SIZE_DAT];
} CHAT_MSG;

// ====================

// ======== 지안 ======== //
// 공통 메시지 형식
// sizeof(COMM_MSG) == 256
typedef struct _COMM_MSG
{
	int  type;
	char dummy[SIZE_DAT];
} COMM_MSG;

// 로그인할때 ID 형식
// sizeof(DRAWLINE_MSG) == 256
typedef struct ID
{
	int  type;		// 로그인할때 ID 형식
	char msg[SIZE_DAT];	// id 내용
} ID_MSG;
// ====================== //


// ========== 지윤 ==========
// 다이얼로그 프로시저
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ==========================

// 소켓 정보 관리 함수
bool AddSocketInfoTCP(SOCKET sock);
bool AddSocketInfoUDP(SOCKADDR_IN addr);
void RemoveSocketInfo(SOCKET sock);
void addMessage(char* message);
SOCKETINFO* GetSocketInfo(SOCKET sock);

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);