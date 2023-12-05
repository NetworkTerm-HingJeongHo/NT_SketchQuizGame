#pragma once

// ======================= 지윤 =======================
#define MAX_USERS 10 // 최대 사용자 수
static int			numUsers; // 현재 사용자 수
static _TCHAR*		userIDs[MAX_USERS];
static _TCHAR*		drawingUserID;

// 타입 정의
// ======================= 정호 =======================

#define PI	3.1416

// 타원
// sizeof(DRAWELLIPSE_MSG) == 256
typedef struct _DRAWELLIPSE_MSG
{
	int  type;
	int	 width;
	int  color;
	int  x0, y0;
	int  x1, y1;
	char dummy[SIZE_TOT - 7 * sizeof(int)];
} DRAWELLIPSE_MSG;

// 다각형
typedef struct _DRAWPOLYGON_MSG
{
	int type;
	int width;
	int color;
	int startX, startY;
	int endX, endY;
	char dummy[SIZE_TOT - 7 * sizeof(int)];
} DRAWPOLYGON_MSG;

// 그림 세부 정보에 대한 구조체
typedef struct _DRAW_DETAIL_INFORMATION
{
	int width = 1;	// 굵기
	int color = RGB(255,0,0);	// 색상
} DRAW_DETAIL_INFORMATION;

// 기능 정의
// ======================= 지윤 =======================
// 대화 상자를 만들고 표시하는 함수
void CreateAndShowDialog(HWND hWnd);
void SelectPenColor(DRAW_DETAIL_INFORMATION* g_drawDetailInformation);
void AddLineWidthOption(HWND hDlg);
void SelectLineWidth(HWND hDlg, DRAW_DETAIL_INFORMATION* g_drawDetailInformation);
void AddUser(_TCHAR* userIDs[], _TCHAR* newUserID);
_TCHAR* SetRandomUserID(_TCHAR* userIDs[]);
void DisplayDrawingUserID(HWND hDlg, _TCHAR* userIDs[]);


// ======================= 정호 =======================

// 도형 옵션 선택 항목 추가
void AddFigureOption(HWND hDlg);

// 그리기 옵션 선택
void SelectFigureOption(HWND hDlg, int &g_currentSelectFigureOption);

// 타원 그리기 과정 실행
void DrawEllipseProcess(HWND hWnd, HDC& hDCMem, WPARAM wParam, LPARAM lParam, DRAW_DETAIL_INFORMATION drawDetailInformation);

// 타원을 특정 HDC에 그림
void DrawEllipseInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 선 그리기 과정 실행
void DrawLineProcess(HWND hWnd, HDC& hDCMem, WPARAM wParam, LPARAM lParam, DRAW_DETAIL_INFORMATION drawDetailInformation);

// 선을 특정 HDC에 그림
void DrawLineInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 다각형 그리는 과정 실행
void DrawPolygonProcess(HWND hWnd, HDC& hDCMem, WPARAM wParam, LPARAM lParam, DRAW_DETAIL_INFORMATION drawDetailInformation, int type);

// 사각형을 특정 HDC에 그림
void DrawRectangleInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 삼각형을 특정 HDC에 그림
void DrawTriangleInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 다각형을 특정 HDC에 그림
void DrawPolygonInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam, int type);

// 직선을 특정 HDC에 그림
void DrawStraightInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 오각형을 특정 HDC에 그림
void DrawPentagonInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 별을 특정 HDC에 그림
void DrawStarInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 포인트 개수에 따라 일정 각도별 위치들 반환
void GetPositionByPoints(int**& positions, int points, int centerX, int centerY, int radius, int revision);

// 사다리꼴을 특정 HDC에 그림
void DrawTrapezoidInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 밤톨을 특정 HDC에 그림
void DrawChestnutInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 평행사변형을 특정 HDC에 그림
void DrawParallelogramInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 마름모를 특정 HDC에 그림
void DrawDiamondInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);

// 화살표를 특정 HDC에 그림
void DrawArrowInHDC(HDC tHDC, WPARAM wParam, LPARAM lParam);