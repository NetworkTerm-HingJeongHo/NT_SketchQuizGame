#include "stdafx.h"

// ============ 정호 ==============

// 해당 byte만큼 받기
int recvn(SOCKET s, char* buf, int len, int flags, SOCKADDR_IN serveraddr, bool bIsUDP)
{
	int received;
	char* ptr = buf;
	int left = len;


	while (left > 0) 
	{
		// UDP 통신으로 데이터 받기
		if (bIsUDP)
		{
			int serveraddrLen = sizeof(serveraddr);
			received = recvfrom(s, ptr, left, flags, (SOCKADDR*)&serveraddr, &serveraddrLen);
		}
		// TCP 통신으로 데이터 받기
		else
		{
			received = recv(s, ptr, left, flags);
		}
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}

		left -= received;
		ptr += received;
	}

	return (len - left);
}

// 해당 byte만큼 보내기
int sendn(SOCKET s, char* buf, int len, int flags, SOCKADDR_IN serveraddr, bool bIsUDP)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) 
	{
		// UDP 통신으로 데이터 보내기
		if (bIsUDP)
		{
			received = sendto(s, ptr, left, flags, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		}
		// TCP 통신으로 데이터 받기
		else
		{
			received = send(s, ptr, left, flags);
		}
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}

		left -= received;
		ptr += received;
	}

	return (len - left);
}

// 고정 크기 : 가변 크기를 알려주는 메시지 전송
void sendMsgLen(SOCKET s, int len, SOCKADDR_IN serveraddr, bool bIsUDP)
{
	// g_sock은 static으로 전역변수이지만, 매개변수 없이 사용하면, 값이 넘어오지 않음.
	int retval;
	retval = sendn(s, (char*)&len, sizeof(int), 0, serveraddr, bIsUDP);
	if (retval == SOCKET_ERROR)
	{
		err_display("sendMsgSize()");
	}
}