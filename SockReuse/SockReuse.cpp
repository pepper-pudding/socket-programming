// SockReuse.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 접속 대기 소켓을 생성할 수 없습니다.");
		return 0;
	}

	//※ 바인딩 전에 IP주소와 포트를 재사용하도록 소켓 옵션을 변경한다.
	 
	// SO_REUSEADDR 이렇게 재사용 옵션을 주면 IP 주소와 PORT 번호가 같아도
	// 에러가 발생하지 않고 bind()가 이루어진다.
	// 서버가 죽은 경우, 같은 IP 주소와 PORT 번호로 연결된 다음 서버로 연결된다.

	// SO_REUSEADDR 옵션(IP 주소와 PORT 번호를 재사용하는 옵션)을 사용하는 경우
	// 1. 서버의 안정적인 상황을 유지하기 위해서.
	// 2. 소켓을 닫는 쪽에서는 TIME_WAIT 상태를 필연적으로 거치게 되는데,
	// 이는 짧게는 수 초에서 길게는 몇 분씩 걸린다.
	// 만약 서버가 죽어서 소켓을 닫는 경우에 이렇게 TIME_WAIT 상태에 빠지는 상황이 생겨서
	// 서버 재시작이 안 되는 상황을 방지하기 위해 SO_REUSEADDR 옵션을 사용할 때가 있다.


	BOOL bOption = TRUE;
	if (::setsockopt(hSocket, SOL_SOCKET,
		SO_REUSEADDR, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR)
	{
		puts("ERROR: 소켓 옵션을 변경할 수 없습니다.");
		return 0;
	}

	//바인딩
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	//	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// SO_REUSEADDR 옵션(IP 주소와 PORT 번호를 재사용하는 옵션)을 사용하는 경우,
	// 아무 IP 주소나 다 연결할 수 있게 하는 INADDR_ANY는 사용하면 안 된다.
	// 왜냐하면 잘못된 서버로 연결되는 경우가 생길 수 있기 때문이다.

	// 예를 들어서 랜 카드(NIC)가 여러 개인 경우 IP 주소가 2개일 수도 있다.
	// 주소가 1.1.1.1, 2.2.2.2 이렇게 있고 서버 프로세스 A, 서버 프로세스 B 이렇게 띄웠을 때
	// 소켓에 SO_REUSEADDR 옵션을 적용하고 INADDR_ANY, 아무 IP 주소를 다 받는 경우,
	// A 서버가 죽었을 때 어떤 주소로 들어오든 포트 번호만 같으면 받기 때문에
	// B 서버로 데이터가 전송될 수가 있다.

	svraddr.sin_addr.S_un.S_addr = inet_addr("192.168.77.2");
	if (::bind(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: 소켓에 IP주소와 포트를 바인드 할 수 없습니다.");
		return 0;
	}

	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
		return 0;
	}

	puts("Echo 서버를 시작합니다.");

	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	while ((hClient = ::accept(hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	{
		puts("새 클라이언트가 연결되었습니다.");
		while ((nReceive = ::recv(hClient,
			szBuffer, sizeof(szBuffer), 0)) > 0)
		{
			::send(hClient, szBuffer, sizeof(szBuffer), 0);
			puts(szBuffer);
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		::closesocket(hClient);
		puts("클라이언트 연결이 끊겼습니다.");
	}

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}


