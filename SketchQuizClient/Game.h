#pragma once

#include "stdafx.h"




void gameStart(HWND statusTimer, HWND statusWord);//게임 시작하는 함수
void Display(HWND status, const char* fmt, ...);//EditText부분에 Text 표시
DWORD WINAPI TimerThread(LPVOID arg); //타이머 스레드
DWORD WINAPI GameThread(LPVOID arg);  //게임 스레드
void newRound();  // 새 라운드 진행 시 실행
