#pragma once

#define SERVERIP4_CHAR_UDP1 "235.7.8.18" // 그룹 1
#define SERVERIP4_CHAR_UDP2 "235.7.8.19" // 그룹 1
#define SERVERPORT 9000
#define BUFSIZE    256

// =========== 지윤 =============
#define CLIENTOUT 3001

// ==== 연경 ====
#define TYPE_CHAT   5000
#define TYPE_ENTER    5001              // 메시지 타입: 입장했을 경우->서버가 이를 알고 이전채팅내용전송
#define TYPE_NOTY     5002  // 입장, 퇴장, 새 라운드 알림 메시
#define TYPE_START    5003
// =============

//==== 지안 ===== //
#define SIZE_TOT 256                    // 전송 패킷(헤더 + 데이터) 전체 크기
#define SIZE_DAT (SIZE_TOT-2*sizeof(int)) // 헤더를 제외한 데이터 부분만의 크기

#define TYPE_ID		2000				// 메시지 타입: id (지안)
#define TYPE_ID_RESULT 2001				// 메시지 티입 : id 결과
#define TYPE_NOTICE		2002			// 메시지 타입 : 공지사항
// ============== //

// ==== 정호 ====
#define WM_SOCKET (WM_USER+1)

// UDP 그룹 번호
#define TYPE_GROUP_A		1000000
#define TYPE_GROUP_B		2000000

// UDP 클라 정보
typedef struct _UDPINFO
{
	SOCKADDR_IN addr;
	int groupNum = 0;
} UDPINFO;

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
	int	 groupNum;
	char msg[SIZE_DAT];
} CHAT_MSG;

// ====================

// ======== 지안 ======== //
// 공통 메시지 형식
// sizeof(COMM_MSG) == 256
typedef struct _COMM_MSG
{
	int  type;
	int	 groupNum;
	char dummy[SIZE_DAT];
} COMM_MSG;

// 로그인할때 ID 형식
// sizeof(DRAWLINE_MSG) == 256
typedef struct ID
{
	int  type;		// 로그인할때 ID 형식
	char msg[SIZE_DAT];	// id 내용
} ID_MSG;

// 로그인 버튼 누르고 최종 저장할때 ID 결과 저장 형식
typedef struct ID_RESULT
{
	int  type;		// '로그인' 버튼 눌렀을때 ID 형식
	char msg[SIZE_DAT];	// id 내용
} ID_RESULT_MSG;

// 공지사항 저장 형식
typedef struct NOTICE
{
	int  type;		// 공지사항 타입 (2002)
	char msg[SIZE_DAT];	// 공지사항 내용
} NOTICE_MSG;
// ====================== //


// ========== 지윤 ==========
// 다이얼로그 프로시저
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ==========================

// 소켓 정보 관리 함수
bool AddSocketInfoTCP(SOCKET sock);
bool AddSocketInfoUDP(SOCKADDR_IN addr, int groupNum);
void RemoveSocketInfo(SOCKET sock);
void addMessage(char* message);
SOCKETINFO* GetSocketInfo(SOCKET sock);

// 해당 클라이언트의 그룹 정보 가져오기
int GetGroupNumber(SOCKADDR_IN addr);


// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// // =========== 정호 =============
// 클라이언트 관리 배열
static int nTotalSockets = 0;
static int nTotalUDPSockets = 0;
static SOCKETINFO* SocketInfoArray[FD_SETSIZE]; //TCP 유저들 있는 변수
static UDPINFO* UDPSocketInfoArray[FD_SETSIZE]; //UDP 유저들 있는 변수

static SOCKET listen_sock4;
static SOCKADDR_IN serveraddr;
static SOCKET socket_UDP;

// ============= 연경 =============== 
//char* g_msgQueue[BUFSIZE];    // 메시지 원형 큐: 이전 대화내용 표시. 꽉 차면 가장 오래된 메시지부터 지워진다.
//int head = 0, tail = 0;           // 원형 큐 인덱스
static MESSAGEQUEUE g_msgQueue;