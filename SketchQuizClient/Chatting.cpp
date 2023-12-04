#include "stdafx.h"

void MySendFile(char* senderName, char* reciverName, char* msg) {

	FILE* fp;
	//char* fileName, *tmp;
	//fileName = strcat((char*)"FROM_", senderName);
	//tmp = strcat((char*)"_TO_", reciverName);
	//fileName = strcat(fileName, tmp);
	//fileName = strcat(fileName, (char*)"txt");

	if ((fp = fopen("test.txt", "a")) == NULL) {
		printf("파일 읽기 오류! \n");
	}

	fwrite(msg, sizeof(char*), sizeof(msg), fp);

	int retval;

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];

	int len;
	while (len = fread(buf, sizeof(char), BUFSIZE, fp)) {   // 파일 끝까지 읽는다
		// BUFSIZE만큼 파일을 읽어들여 server로 보내는 것이 한 번의 while 루틴이다.
		// 
		// 데이터 보내기(고정 길이)
		retval = send(g_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		// 데이터 보내기(가변 길이)
		retval = send(g_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d+%d바이트를 "
			"보냈습니다.\n", sizeof(int), retval);
	}

	fclose(fp);
}

void printMessageQueue(MESSAGEQUEUE msgQueue) {
	int idx = msgQueue.head;
	for (int i = 0; i < ((msgQueue.tail - msgQueue.head + BUFSIZE) % BUFSIZE); i++) {
		DisplayText(msgQueue.queue[idx]);
		DisplayText("\r\n");
		idx = (idx + 1) % BUFSIZE;
	}
}
