#pragma once

/*
	지안 - 2000번대
	지윤 - 3000번대
	정호 - 4000번대
	연경 - 5000번대
*/
#define TYPE_CHAT     5000              // 메시지 타입: 채팅
#define TYPE_DRAWLINE 1001              // 메시지 타입: 선 그리기
#define TYPE_ERASEPIC 1002              // 메시지 타입: 그림 지우기
#define TYPE_Queue    1004              // 메시지 타입: 메시지 큐(연경)
#define TYPE_ENTER    5001              // 메시지 타입: 입장했을 경우->서버가 이를 알고 이전채팅내용전송
#define TYPE_NOTY     5002  // 입장, 퇴장, 새 라운드 알림 메시지 출력
// ====== 지안 ========= //
#define TYPE_ID			2000				// 메시지 타입: id (지안)
#define TYPE_ID_RESULT	2001				// 메시지 티입 : id 결과
// ==================== //

#define WM_DRAWLINE (WM_USER+1)         // 사용자 정의 윈도우 메시지(1)
#define WM_ERASEPIC (WM_USER+2)         // 사용자 정의 윈도우 메시지(2)

#define SIZE_TOT 256                    // 전송 패킷(헤더 + 데이터) 전체 크기
#define SIZE_DAT (SIZE_TOT- 2*sizeof(int)) // 헤더를 제외한 데이터 부분만의 크기

// 공통 메시지 형식
// sizeof(COMM_MSG) == 256
typedef struct _COMM_MSG
{
	int  type;
	int	 groupNum;
	char dummy[SIZE_DAT];
} COMM_MSG;

// 채팅 메시지 형식
// sizeof(CHAT_MSG) == 256
typedef struct _CHAT_MSG
{
	int  type;
	int	 groupNum;
	char msg[SIZE_DAT];
} CHAT_MSG;

// 선 그리기 메시지 형식
// sizeof(DRAWLINE_MSG) == 256
typedef struct _DRAWLINE_MSG
{
	int  type;
	int	 groupNum;
	int  width;
	int  color;
	int  x0, y0;
	int  x1, y1;
	char dummy[SIZE_TOT - 8 * sizeof(int)];
} DRAWLINE_MSG;

// 그림 지우기 메시지 형식
// sizeof(ERASEPIC_MSG) == 256
typedef struct _ERASEPIC_MSG
{
	int  type;
	int	 groupNum;
	char dummy[SIZE_DAT];
} ERASEPIC_MSG;

// ======= 연경 ======= 
typedef struct _MESSAGEQUEUE {
	int type;
	char queue[10][20] = { NULL };         // 메시지 원형 큐: 이전 대화내용 표시. 꽉 차면 가장 오래된 메시지부터 지워진다.
	int head = 0;                 // 원형 큐 인덱스
	int tail = 0;
} MESSAGEQUEUE;

// ======== 지안 ======== //
// 로그인할때 ID 형식
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
// ====================== //

// 메인 윈도우(첫화면) 프로시저
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 대화상자(그림판) 프로시저
INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 자식 윈도우 프로시저
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI ReadThread(LPVOID arg);
DWORD WINAPI WriteThread(LPVOID arg);

// 에디트 컨트롤 출력 함수
void DisplayText(const char *fmt, ...);
// 게임 시작 버튼 onClick()함수
void btnGameStart_click();
