#include "stdafx.h"

int main(int argc, char* argv[])
{
	// ========= ��ȣ ========
	// ������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("MyWndClass");
	if (!RegisterClass(&wndclass)) return 1;

	// �ӽ� ������ ����
	HWND hWnd = CreateWindow(_T("MyWndClass"), _T("TCP ����"), WS_OVERLAPPEDWINDOW,
		0, 0, 600, 300, NULL, NULL, NULL, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	/*----- TCP/IPv4 ���� �ʱ�ȭ ���� -----*/
	// ���� ����
	listen_sock4 = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock4 == INVALID_SOCKET) err_quit("socket()");

	// bind()
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock4, (struct sockaddr*)&serveraddr,
		sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock4, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// TCP/IPv4 ���Ͽ� ����ŷ ��� ����
	u_long nonBlockingModeOn = 1;
	retval = ioctlsocket(listen_sock4, FIONBIO, &nonBlockingModeOn);
	if (retval == SOCKET_ERROR) {
		err_quit("ioctlsocket()");
	}
	/*----- TCP/IPv4 ���� �ʱ�ȭ ���� -----*/

	 /*----- UDP/IPv4 ���� �ʱ�ȭ ���� -----*/
   // TODO: ������ �����ϰ� �ʱ�ȭ�Ѵ�. == ��ȣ ==

	socket_UDP = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_UDP == INVALID_SOCKET)
	{
		err_quit("socket()");
	}

	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq1;
	inet_pton(AF_INET, SERVERIP4_CHAR_UDP1, &mreq1.imr_multiaddr.s_addr); // �����ϰų� Ż���� IPv4 ��Ƽ�ɽ�Ʈ address(�ּ�) (������ ���Ƹ�)
	mreq1.imr_interface.s_addr = htonl(INADDR_ANY);      // ���� ip address (��)
	retval = setsockopt(socket_UDP, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char*)&mreq1, sizeof(mreq1));
	if (retval == SOCKET_ERROR) err_quit("setsockopt() 1");

	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq2;
	inet_pton(AF_INET, SERVERIP4_CHAR_UDP2, &mreq2.imr_multiaddr.s_addr); // �����ϰų� Ż���� IPv4 ��Ƽ�ɽ�Ʈ address(�ּ�) (������ ���Ƹ�)
	mreq2.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(socket_UDP, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char*)&mreq2, sizeof(mreq2));
	if (retval == SOCKET_ERROR) err_quit("setsockopt() 2");

	// ---------------- ���� ---------------- //
	// bind() UDP �׷� B bind

	retval = bind(socket_UDP, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// --------------------------------------- //


	/*----- UDP/IPv4 ���� �ʱ�ȭ ���� -----*/

	/*----- UDP/IPv6 ���� �ʱ�ȭ ���� -----*/
	// TODO: ������ �����ϰ� �ʱ�ȭ�Ѵ�.
	/*----- UDP/IPv6 ���� �ʱ�ȭ ���� -----*/

	// ������ ��ſ� ����� ����(����)
	fd_set rset;
	SOCKET client_sock;
	int addrlen;
	// ������ ��ſ� ����� ����(IPv4)
	struct sockaddr_in clientaddr4;
	// ������ ��ſ� ����� ����(IPv6)
	struct sockaddr_in6 clientaddr6;

	// ========== ��ȣ ==========
	int recvLen; // ���� ���� ������ ũ��
	int sendLen; // ���� ���� ������ ũ��

	// WSAAsyncSelect()

	// TCP�� ������ �ؾ��ϹǷ� FD_ACCEPT�� �߰�
	retval = WSAAsyncSelect(listen_sock4, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	// UDP�� TCP�� �޸� ������ �ʿ�����Ƿ�
	// FD_ACCEPT�� ���� ����.
	// FD_READ�� �����͸� ������ �� �ֵ��� ����
	retval = WSAAsyncSelect(socket_UDP, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ���� ����
	WSACleanup();
	return 0;
}


// ������ �޽��� ó��
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		// ================= ���� ================= 
	case WM_CREATE:
		/* Ŭ���̾�Ʈ ��� */
		InitializeListView(hWnd);
		/* ä�� ������ */
		InitializeChatListView(hWnd);
		return 0;
		// ============ ��ȣ ============
	case WM_SOCKET: // ���� ���� ������ �޽���
		ProcessSocketMessage(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		// ==============================
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ���� ���� ������ �޽��� ó��
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//FILE* fd;
	// ������ ��ſ� ����� ����
	SOCKETINFO* ptr;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;
	char buf[BUFSIZE + 1];

	// === ��ȣ ===
	int groupNumUDP = 0;

	// ���� �߻� ���� Ȯ��
	if (WSAGETSELECTERROR(lParam)) {
		err_display(WSAGETSELECTERROR(lParam));
		RemoveSocketInfo(wParam);
		return;
	}

	// �޽��� ó��
	switch (WSAGETSELECTEVENT(lParam)) {
		// ����
	case FD_ACCEPT:
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock4, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			return;
		}
		else {
			// ������ Ŭ���̾�Ʈ ���� ���
			printf("\n[TCP/IPv4 ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

			// ���� ���� �߰�
			AddSocketInfoTCP(client_sock);
			retval = WSAAsyncSelect(client_sock, hWnd,
				WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
			if (retval == SOCKET_ERROR) {
				err_display("WSAAsyncSelect()");
				RemoveSocketInfo(client_sock);
			}
		}
		break;
	case FD_READ:
		// TCP socket
		printf("FD_READ\n");
		if (wParam != socket_UDP)
		{
			ptr = GetSocketInfo(wParam);
			//if (ptr->recvbytes > 0) {
			//	printf("no");
			//	return;
			//}
			// ���� ������ �ޱ�
			retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
			// ============================== ���� ================================//
			// COMM_MSG Ÿ������ ����ȯ (�⺸Ÿ��) -> ����ü type�� ���� �����̴�.
			COMM_MSG* comm_msg;
			comm_msg = (COMM_MSG*)&(ptr->buf);
			printf("[COMM_MSG type] %d\n", comm_msg->type); //���� type ���

			// Type�� ���� �ٸ� ����ü�� ���� switch (���� ����ȯ ����� ��)
			switch (comm_msg->type) {
			case (TYPE_ID):	// TYPE_ID �� ��� (id ���)
				// ***  ����ȯ *** //
				ID_MSG* id_msg;
				id_msg = (ID_MSG*)&(ptr->buf); // ID�� ����ȯ
				printf("[TYPE_ID ���� ������] %s\n", id_msg->msg);
				// *** ����ȯ �� *** //

				// ���� ���� ���� �г����� �ߺ����� �ʾҴٸ� (ture)
				if (CheckIDDuplication(nTotalSockets, SocketInfoArray, id_msg->msg)) {
					printf("[TCP] �ߺ��� ���̵� ����");
					// ---- Ŭ���̾�Ʈ�� ���� ------//
					size_t dataSize = strlen("true"); // �ߺ��� �ƴϹǷ�, "true" Ŭ���̾�Ʈ�� ����
					retval = send(ptr->sock, "true", dataSize, 0);

					if (retval == SOCKET_ERROR) {
						err_display("send()");
						//break;
					}
					// ----------------------------//
				}
				else {	// id �ߺ��� �ִٸ�, ��� ����
					printf("[TCP] ID�� �ߺ��˴ϴ�. ��� �����Դϴ�.");
					// ---- Ŭ���̾�Ʈ�� ���� ------//
					size_t dataSize = strlen("false"); //�ߺ��̹Ƿ�, "false" Ŭ���̾�Ʈ�� ����
					retval = send(ptr->sock, "false", dataSize, 0);

					if (retval == SOCKET_ERROR) {
						err_display("send()");
						//break;
					}
					// ----------------------------//

					// ---------���� ����� -------//

					// RemoveSocketInfo(ptr->sock);
					// ----------------------------//
				}

				break;
			case (TYPE_NOTICE): // TYPE_NOTICE�� ��� (�������� ���� ���)
				NOTICE_MSG* notice_msg;
				notice_msg = (NOTICE_MSG*)&(ptr->buf); // NOTICE_MSG : ������������ ����ȯ
				printf("[TYPE_NOTICE ���� ������] %s\n", notice_msg->msg);
				break;
			case (TYPE_ID_RESULT):	// TYPE_ID �� ��� (id ���)
				// ***  ����ȯ *** //
				ID_RESULT_MSG* id_result_msg;
				id_result_msg = (ID_RESULT_MSG*)&(ptr->buf); // ID�� ����ȯ
				printf("[TYPE_RESULT_ID ���� ������] %s\n", id_result_msg->msg);
				// *** ����ȯ �� *** //

				// --------- ���� �г���(ID) �����ϰ�, gui�� �ø��� --------- //
				setIDInSocket(id_result_msg->msg, ptr); //id ���
				printf("[TCP] TYPE_RESULT_ID, ���� ���� �г��� ��ϿϷ� : %s\n", ptr->id_nickname_char);
				printf("[TCP] TYPE_RESULT_ID, ���� ���� port ��ϿϷ� : %d\n", ptr->sin_port);
				printf("[TCP] TYPE_RESULT_ID, ���� ���� �ּ�(char) : %s\n", inet_ntoa(ptr->sin_addr));
				//MessageBox(NULL, ptr->id_nickname, _T("���� ���� �г��� ��ϿϷ�(_TCHAR)"), MB_ICONERROR);
				// =========== ���� ============
				AddClientToListView(ptr->sin_port, ptr->id_nickname, ptr->sock);
				// =============================

				// -------------------------------------------------------- //
				break;
				// ======== ���� =======
				//case TYPE_CHAT:
				//	fd = fopen("chatting_log.txt", "a");
				//	CHAT_MSG* chat_msg;
				//	chat_msg = (CHAT_MSG*)comm_msg;
				//	fwrite(chat_msg->msg, sizeof(char*), sizeof(chat_msg->msg), fd);
				//	fclose(fd);
				// ==================
			default:
				break;
			}

			//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
			//printf("[���� ������] %s\n", ptr->buf);
			// ================================================================== //
			// 
			// ======== ���� =======
			addMessage(ptr->buf);
			// ====================

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				RemoveSocketInfo(wParam);
				return;
			}
			printf("[TCP] ������ ���� : %d, ������ : %s\n", retval, buf);
			// ���� ����Ʈ �� ����
			ptr->recvbytes += retval;

			// ä�� �����͸� ǥ���Ѵ�.
		}
		// UDP socket
		else
		{
			// ������ �ޱ�
			addrlen = sizeof(clientaddr);
			retval = recvfrom(socket_UDP, buf, BUFSIZE, 0, (SOCKADDR*)&clientaddr, &addrlen);
			printf("UDP groupNum : %d, type : %d\n", ((COMM_MSG*)&buf)->groupNum, ((COMM_MSG*)&buf)->type);


			groupNumUDP = GetGroupNumber(clientaddr);
			printf("Get Group Num : %d\n", groupNumUDP);

			if (groupNumUDP == -1)
			{
				// UDP�� ������ Ŭ�� ���� ����
				AddSocketInfoUDP(clientaddr, ((COMM_MSG*)&buf)->groupNum, ((COMM_MSG*)&buf)->dummy);
			}

			printf("[UDP] ������ ���� : %d, ������ : %s\n", retval, ((COMM_MSG*)&buf)->dummy);
			char msg[256];
			strcpy(msg, ((COMM_MSG*)&buf)->dummy);
			if (retval == SOCKET_ERROR) {
				err_display("recvfrom()");
				return;
			}
			// ======== ���� =======
			COMM_MSG* comm_msg = (COMM_MSG*)&buf;
			switch (comm_msg->type) {
			case TYPE_NOTY:
			case TYPE_ENTER: { //�����ߴٴ� �޽����� ��� �ش� Ŭ���̾�Ʈ���� ���� �޽��� ���� ����
				// ================= ���� =================
				ClearChatListView();
				// ========================================
				
				UDPINFO* clientUDP = UDPSocketInfoArray[nTotalSockets - 1];  //���� �ֱ� ������ ����
				COMM_MSG sendMsg;
				sendMsg.type = TYPE_CHAT;
				FILE* sendFd;
				if(comm_msg->groupNum== TYPE_GROUP_A)
					sendFd = fopen("chatting_log_1.txt", "r");
				else 
					sendFd = fopen("chatting_log_2.txt", "r");
				
				printf("======== ���� ä�� ���� ======= \n");
				while (fgets(sendMsg.dummy, BUFSIZE, sendFd)) {
					printf("%s\n", sendMsg.dummy);
					// ================= ���� =================
					AddChatMessageToListView(sendMsg.dummy);
					// ========================================
					// ������ ������
					if (groupNumUDP == clientUDP->groupNum)
					{
						// ������ ������
						retval = sendto(socket_UDP, (char*)&sendMsg, BUFSIZE, 0, (SOCKADDR*)&clientUDP->addr, sizeof(clientUDP->addr));
						//if (retval == SOCKET_ERROR) {
						//	err_display("sendto()");
						//	return;
						//}
						//printf("sendto retval : %d\n", retval);
					}
					
					//if (retval == SOCKET_ERROR) {
					//	err_display("sendto()");
					//	return;
					//}
				}
				printf("=========================\n");
				fclose(sendFd);
				break;
			}
			case TYPE_START: {
				int randomIndex;
				while (true) {

					// �õ尪 ����
					srand((unsigned)time(NULL));

					// ���� �ε��� ����
					randomIndex = rand() % nTotalSockets;
					if (UDPSocketInfoArray[randomIndex]->groupNum == groupNumUDP) break;  // ���� �׷� ������ ���� ������ ��� ����������
				}
				// �������� ���õ� ����� ���̵� ��ȯ
				char* selectedName = UDPSocketInfoArray[randomIndex]->id_nickname;

				for (int i = 0; i < nTotalUDPSockets; i++)
				{
					COMM_MSG sendMsg;
					sendMsg.type = TYPE_SELECT;
					strcpy(sendMsg.dummy, selectedName);
					// ���� �׷쿡�� ������ ����
					UDPINFO* clientUDP = UDPSocketInfoArray[i];
					printf("send groupNumUDP : %d, clientUDP->GroupNum : %d\n", groupNumUDP, clientUDP->groupNum);
					if (groupNumUDP == clientUDP->groupNum)
					{
						// ������ ������
						retval = sendto(socket_UDP, (char*)&sendMsg, BUFSIZE, 0, (SOCKADDR*)&clientUDP->addr, sizeof(clientUDP->addr));
						if (retval == SOCKET_ERROR) {
							err_display("sendto()");
							return;
						}
						printf("sendto retval : %d\n", retval);
					}
				}
				break;
			}


			case TYPE_CHAT: {
				FILE* fd;
				if (comm_msg->groupNum == TYPE_GROUP_A)
					fd = fopen("chatting_log_1.txt", "a");
				else
					fd = fopen("chatting_log_2.txt", "a");
				char n = '\n';
				fwrite(msg, sizeof(char), strlen(msg), fd);
				fwrite(&n, sizeof(char), sizeof(n), fd);
				// ================= ���� =================
				AddChatMessageToListView(msg);
				// ========================================
				fclose(fd);
				break;
			}

			}

			// ====================
		}
	case FD_WRITE:
		// UDP ������ �ƴ� ��� (TCP�� ���)
		if (wParam != socket_UDP)
		{
			ptr = GetSocketInfo(wParam);
			//for (int i = 0; i < nTotalSockets; i++) {
			//	SOCKETINFO* ptr = SocketInfoArray[i];
			if (ptr->recvbytes == BUFSIZE) {
				// ���� ����Ʈ �� ����
				ptr->recvbytes = 0;

				// ���� ������ ��� Ŭ���̾�Ʈ���� �����͸� ����!
				for (int j = 0; j < nTotalSockets; j++) {
					SOCKETINFO* ptr2 = SocketInfoArray[j];
					retval = send(ptr2->sock, ptr->buf, BUFSIZE, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						RemoveSocketInfo(j);
						--j; // ���� �ε��� ����
						continue;
					}
				}
			}
			//}
		}
		// UDP socket
		else
		{
			for (int i = 0; i < nTotalUDPSockets; i++)
			{
				// ���� �׷쿡�� ������ ����
				UDPINFO* clientUDP = UDPSocketInfoArray[i];
				printf("send groupNumUDP : %d, clientUDP->GroupNum : %d\n", groupNumUDP, clientUDP->groupNum);
				if (groupNumUDP == clientUDP->groupNum)
				{
					// ������ ������
					retval = sendto(socket_UDP, buf, BUFSIZE, 0, (SOCKADDR*)&clientUDP->addr, sizeof(clientUDP->addr));
					if (retval == SOCKET_ERROR) {
						err_display("sendto()");
						return;
					}
					printf("sendto retval : %d\n", retval);
				}
			}
		}
		break;
	case FD_CLOSE:
		RemoveSocketInfo(wParam);
		break;
	}
}

// ���� ���� ���
SOCKETINFO* GetSocketInfo(SOCKET sock)
{
	// ���� ������ Ŭ���̾�Ʈ �߿��� ��ġ�ϴ� ���� Ž��
	for (int i = 0; i < nTotalSockets; i++)
	{
		SOCKETINFO* ptr = SocketInfoArray[i];
		// ã���� ���, �ش� ���� ��ȯ
		if (ptr->sock == sock)
		{
			return ptr;
		}
	}
	return NULL;
}

// UDP Ŭ�� ���� �߰�
bool AddSocketInfoUDP(SOCKADDR_IN addr, int groupNum, char* id_nickname)
{
	// ������ ������ ���� �ִ� �������� Ȯ��
	for (int i = 0; i < nTotalUDPSockets; i++)
	{
		if (inet_ntoa(UDPSocketInfoArray[i]->addr.sin_addr) == inet_ntoa(addr.sin_addr) &&
			ntohs(UDPSocketInfoArray[i]->addr.sin_port) == ntohs(addr.sin_port) &&
			ntohs(UDPSocketInfoArray[i]->addr.sin_family) == ntohs(addr.sin_family))
		{
			// �׷��� ������ ���
			if ((groupNum != UDPSocketInfoArray[i]->groupNum) && (groupNum == TYPE_GROUP_A || groupNum == TYPE_GROUP_B))
			{
				UDPSocketInfoArray[i]->groupNum = groupNum;
			}
			return false;
		}
	}

	UDPINFO* newUDPClient = (UDPINFO*)malloc(sizeof(UDPINFO));
	if (newUDPClient == NULL)
	{
		return false;
	}
	newUDPClient->addr = addr;
	newUDPClient->groupNum = groupNum;
	strcpy(newUDPClient->id_nickname, id_nickname);

	// UDP Ŭ�� ���� �߰�
	UDPSocketInfoArray[nTotalUDPSockets++] = newUDPClient;

	printf("Add UDP Client Info groupNum : %d\n", newUDPClient->groupNum);
	return true;
}

// TCP ���� ���� �߰�
bool AddSocketInfoTCP(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return false;
	}
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return false;
	}
	ptr->sock = sock;
	ptr->recvbytes = 0;

	//========== ���� =============//
	// Ŭ���̾�Ʈ ���� ���
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (struct sockaddr*)&clientaddr, &addrlen);

	ptr->sin_addr = clientaddr.sin_addr;	// Ŭ���̾�Ʈ �ּ� ����
	ptr->sin_port = ntohs(clientaddr.sin_port);	// Ŭ���̾�Ʈ ��Ʈ��ȣ ����

	// TCP ���� �迭�� �߰�
	SocketInfoArray[nTotalSockets++] = ptr;

	return true;
}

// ���� ���� ����
void RemoveSocketInfo(SOCKET sock)
{
	// Ŭ���̾�Ʈ ���� ���
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (struct sockaddr*)&clientaddr, &addrlen);
	// Ŭ���̾�Ʈ ���� ���
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP/IPv4 ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		addr, ntohs(clientaddr.sin_port));
	// =========== ���� ============
	RemoveClientFromListView(ntohs(clientaddr.sin_port));
	// =============================

	// Ŭ���̾�Ʈ ���� ����
	for (int i = 0; i < nTotalSockets; i++)
	{
		SOCKETINFO* ptr = SocketInfoArray[i];
		// ã���� ���, �ش� ���� ��ȯ
		if (ptr->sock == sock)
		{
			if (i != (nTotalSockets - 1))
			{
				SocketInfoArray[i] = SocketInfoArray[nTotalSockets - 1];
			}
			--nTotalSockets;
		}
	}

	// ���� �ݱ�
	closesocket(sock);
}

// �ش� Ŭ���̾�Ʈ�� �׷� ���� ��������
int GetGroupNumber(SOCKADDR_IN addr)
{
	// ������ ������ ���� �ִ� �������� Ȯ��
	for (int i = 0; i < nTotalUDPSockets; i++)
	{
		if (inet_ntoa(UDPSocketInfoArray[i]->addr.sin_addr) == inet_ntoa(addr.sin_addr) &&
			ntohs(UDPSocketInfoArray[i]->addr.sin_port) == ntohs(addr.sin_port) &&
			ntohs(UDPSocketInfoArray[i]->addr.sin_family) == ntohs(addr.sin_family))
		{
			return UDPSocketInfoArray[i]->groupNum;
		}
	}
	return -1;
}


void addMessage(char* message) {
	if ((g_msgQueue.tail + 1) % BUFSIZE == g_msgQueue.head) { //ť�� ���� ���: 
		g_msgQueue.head = (g_msgQueue.head + 1) % BUFSIZE; //������ ��Ҹ� �ϳ� ����� ���� �ϳ��� Ȯ���Ѵ�.
	}
	strcpy(g_msgQueue.queue[g_msgQueue.tail], message);
	g_msgQueue.tail = (g_msgQueue.tail + 1) % BUFSIZE;
}

// �������� Ŭ���̾�Ʈ�� 
DWORD WINAPI messageQueueThread(LPVOID arg) {
	return 0;
}
