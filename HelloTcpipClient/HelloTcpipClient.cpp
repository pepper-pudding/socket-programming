// HelloTcpipClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


int _tmain(int argc, _TCHAR* argv[])
{
	//※윈속 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	//1. 접속대기 소켓 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 소켓을 생성할 수 없습니다.");
		return 0;
	}

	//2. 포트 바인딩 및 연결
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("192.168.77.2");
	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: 서버에 연결할 수 없습니다.");
		return 0;
	}

	// 2.1 - TCP_NODELAY 옵션을 주는 경우
	// 버퍼에서 대기하는 시간없이 데이터를 전송한다.
	// WireShark에서 확인해보면 PSH로 보내지는 것을 확인할 수 있다.
	// 빠른 반응 속도가 필요한 TCP 기반 게임 등에서 해당 옵션을 사용할 수 있다.
	// 여기서 궁금한 점은, 서버 쪽에도 NODELAY 옵션을 줘야 할까?
	// 보편적인 답으로는 서버 쪽에는 NODELAY 옵션을 줄 필요가 없다.
	// NODELAY 옵션은 클라이언트 쪽에서 많이 설정한다.
	// 이유는 추후 설명.
	int nOpt = 1;
	::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&nOpt, sizeof(nOpt));

	//3. 채팅 메시지 송/수신
	char szBuffer[128] = { 0 };
	while (1)
	{
		//사용자로부터 문자열을 입력 받는다.
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)		break;

		//사용자가 입력한 문자열을 서버에 전송한다.
		::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);
		//서버로부터 방금 보낸 문자열에 대한 에코 메시지를 수신한다.
		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server: %s\n", szBuffer);
	}

	//4. 소켓을 닫고 종료.
	//::shutdown(hSocket, SD_BOTH);
	::closesocket(hSocket);
	//※윈속 해제
	::WSACleanup();
	return 0;
}
