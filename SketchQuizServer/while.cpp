while (1) {
	// 소켓 셋 초기화
	FD_ZERO(&rset);
	FD_SET(listen_sock4, &rset);
	FD_SET(listen_sock6, &rset);
	for (int i = 0; i < nTotalSockets; i++) {
		FD_SET(SocketInfoArray[i]->sock, &rset);
	}

	// select()
	retval = select(0, &rset, NULL, NULL, NULL);
	if (retval == SOCKET_ERROR) err_quit("select()");

	// 소켓 셋 검사(1): 클라이언트 접속 수용
	if (FD_ISSET(listen_sock4, &rset)) { // TCP/IPv4
		addrlen = sizeof(clientaddr4);
		client_sock = accept(listen_sock4,
			(struct sockaddr *)&clientaddr4, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		else {
			// 클라이언트 정보 출력
			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientaddr4.sin_addr, addr, sizeof(addr));
			printf("\n[TCP/IPv4 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				addr, ntohs(clientaddr4.sin_port));
			// 소켓 정보 추가: 실패 시 소켓 닫음
			if (!AddSocketInfo(client_sock, false, false))
				closesocket(client_sock);
		}
	}
	if (FD_ISSET(listen_sock6, &rset)) { // TCP/IPv6
		addrlen = sizeof(clientaddr6);
		client_sock = accept(listen_sock6,
			(struct sockaddr *)&clientaddr6, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		else {
			// 클라이언트 정보 출력
			char addr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &clientaddr6.sin6_addr, addr, sizeof(addr));
			printf("\n[TCP/IPv6 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				addr, ntohs(clientaddr6.sin6_port));
			// 소켓 정보 추가: 실패 시 소켓 닫음
			if (!AddSocketInfo(client_sock, true, false))
				closesocket(client_sock);
		}
	}

	 ====== 연경 ======
	// 새로 들어온 클라이언트에게 이전 대화 내용 전송
	SOCKETINFO* curClientPtr = SocketInfoArray[nTotalSockets - 1];
	int sendLen = sizeof(g_msgQueue);
	// 고정 길이 전송
	retval = sendn(curClientPtr->sock, (char*)&sendLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		continue;
	}

 

	// 가변 길이 전송
	retval = sendn(curClientPtr->sock, (char*)&g_msgQueue, recvLen, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		continue;
	}

	// =========================
	

	// 소켓 셋 검사(2): 데이터 통신
	for (int i = 0; i < nTotalSockets; i++) {
		SOCKETINFO *ptr = SocketInfoArray[i];
		if (FD_ISSET(ptr->sock, &rset)) {
			// 데이터 받기

			// ============ 정호 ============
			// 고정 크기 데이터 받기
			retval = recvn(ptr->sock, (char*)&recvLen, sizeof(int), 0);
			if (retval == 0 || retval == SOCKET_ERROR)
			{
				RemoveSocketInfo(i);
				continue;
			}
			// 가변 크기 데이터 받기
			retval = recvn(ptr->sock, ptr->buf, recvLen, 0);
			if (retval == 0 || retval == SOCKET_ERROR) {
				RemoveSocketInfo(i);
				continue;
			}

			//// 받은 바이트 수 누적
			//ptr->recvbytes += retval;
			//sendLen = ptr->recvbytes;
			//// 받은 바이트 수 리셋
			//ptr->recvbytes = 0;

			// 현재 접속한 모든 클라이언트에 데이터 전송
			for (int j = 0; j < nTotalSockets; j++) {
				SOCKETINFO *ptr2 = SocketInfoArray[j];

				// ======== 정호 ==========
				// 고정 길이 전송
				retval = sendn(ptr2->sock, (char*)&recvLen, sizeof(int), 0);
				if (retval == SOCKET_ERROR) 
				{
					err_display("send()");
					RemoveSocketInfo(j);
					--j; // 루프 인덱스 보정
					continue;
				}

				// 가변 길이 전송
				retval = sendn(ptr2->sock, ptr->buf, recvLen, 0);
				if (retval == SOCKET_ERROR) 
				{
					err_display("send()");
					RemoveSocketInfo(j);
					--j; // 루프 인덱스 보정
					continue;
				}
			}
		}
	} /* end of for */
} /* end of while (1) */