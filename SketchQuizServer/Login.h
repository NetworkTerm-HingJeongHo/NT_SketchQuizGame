#pragma once

void setIDInSocket(char cilentID[], SOCKETINFO* ptr); // ID 소켓에 등록하는 함수.
bool CheckIDDuplication(int nTotalSockets, SOCKETINFO* SocketInfoArray[], char id[]);