#pragma once

// 소켓과 관련된 기능

// =========== 정호 ===========

// 가변 크기 : 해당 byte만큼 받기
int recvn(SOCKET s, char* buf, int len, int flags, SOCKADDR_IN serveraddr, bool bIsUDP);

// 가변 크기 : 해당 byte만큼 보내기
int sendn(SOCKET s, char* buf, int len, int flags, SOCKADDR_IN serveraddr, bool bIsUDP);

// 고정 크기 : 가변 크기를 알려주는 메시지 전송
void sendMsgLen(SOCKET s, int len, SOCKADDR_IN serveraddr, bool bIsUDP);