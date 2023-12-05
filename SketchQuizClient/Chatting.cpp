#include "stdafx.h"

void MySendFile(char* senderName, char* reciverName, char* msg) {

	FILE* fp;
	//char* fileName, *tmp;
	//fileName = strcat((char*)"FROM_", senderName);
	//tmp = strcat((char*)"_TO_", reciverName);
	//fileName = strcat(fileName, tmp);
	//fileName = strcat(fileName, (char*)"txt");
	char fileName[256];
	snprintf(fileName, sizeof(fileName), "From_%s_to_%s.txt", senderName, reciverName);

	if ((fp = fopen(fileName, "a")) == NULL) {
		printf("���� �б� ����! \n");
	}

	fwrite(msg, sizeof(char*), sizeof(msg), fp);

	int retval;

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE];

	int len;
	while (len = fread(buf, sizeof(char), BUFSIZE, fp)) {   // ���� ������ �д´�
		// BUFSIZE��ŭ ������ �о�鿩 server�� ������ ���� �� ���� while ��ƾ�̴�.
		// 
		// ������ ������(���� ����)
		retval = send(g_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		// ������ ������(���� ����)
		retval = send(g_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d+%d����Ʈ�� "
			"���½��ϴ�.\n", sizeof(int), retval);
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
