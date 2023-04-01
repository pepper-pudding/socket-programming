// HelloTcpip.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//※윈속 초기화
	// Windows 플랫폼에서 네트워크 프로그래밍에 사용되는 Winsock 라이브러리를 초기화한다.

	WSADATA wsa = { 0 };
	// WSAStartup 함수는 두 개의 인수로 호출된다.
	// 첫 번째 인수인 MAKEWORD(2, 2)는 응용 프로그램에서 사용할 것으로 예상되는 Winsock의 버전(이 경우 버전 2.2)을 지정한다.
	// 두 번째 인수는 Winsock 구현에 대한 정보를 받는 데 사용되는 WSADATA 구조에 대한 포인터이다.
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	//1. 접속대기 소켓 생성
	// 클라이언트의 접속 요청을 수신할 소켓을 만든다.
	// 클라이언트와 통신할 때 사용하는 소켓과는 별개의 소켓이다.
	// 접속을 받는 서버소켓(hSocket) + 클라이언트와 통신할 통신소켓(hClient)

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0); // 소켓은 IPv4 소켓이며,  TCP 프로토콜을 사용한다.
	// 첫 번째 인수는 L3를 나타낸다. AF_INET = IPv4, AF_INET6 = IPv6                                                    
	// 두 번째 인수는 L4를 나타낸다. SOCK_STREAM = TCP, SOCK_DGRAM = UDP
	// 세 번째 인수는 주어진 소켓 유형에 대한 기본 프로토콜을 지정하는 0으로 설정된다.

	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 접속 대기 소켓을 생성할 수 없습니다.");
		return 0;
	}

	//2. 포트 바인딩
	// 소켓을 특정 포트 번호 및 IP 주소에 바인딩한다.

	SOCKADDR_IN	svraddr = { 0 }; // 소켓 주소에 대한 정보가 포함된 SOCKADDR_IN 구조를 초기화한다.
	svraddr.sin_family = AF_INET; // 소켓이 IPv4 소켓임을 나타낸다.
	svraddr.sin_port = htons(25000); // 포트 번호 : 25000. 포트 번호를 네트워크 바이트 순서로 변환하기 위해 htons 함수를 사용한다.
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 소켓이 시스템에서 사용 가능한 모든 네트워크 인터페이스에 바인딩할 수 있다.
	if (::bind(hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) // bind 함수는 소켓 핸들, sockaddr 구조에 대한 포인터, sockaddr 구조의 크기를 인수로 받는다.
	{
		puts("ERROR: 소켓에 IP주소와 포트를 바인드 할 수 없습니다.");
		return 0;
	}

	//3. 접속대기 상태로 전환
	// 소켓이 들어오는 연결을 수락할 수 있도록 소켓을 LISTEN 상태로 전환한다.
	
	// listen 함수는 두 개의 인수로 호출된다.
	// 첫 번째 인수는 1.에서 생성되고 2.에서 로컬 주소 및 포트 번호에 바인딩된 소켓 핸들인 hSocket이다.
	// 두 번째 인수 SOMAXCONN은 소켓에 대기할 수 있는 보류 중인 연결의 최대 수를 나타내는 특수 값이다.
	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
		return 0;
	}

	//4. 클라이언트 접속 처리 및 대응
	// 클라이언트 연결을 처리하고 응답을 클라이언트로 다시 보내는 데 사용되는 변수를 정의한다.

	SOCKADDR_IN clientaddr = { 0 }; // 클라이언트의 주소와 포트 번호에 대한 정보를 저장한다.
	int nAddrLen = sizeof(clientaddr); // clientaddr 구조의 크기를 저장한다.
	SOCKET hClient = 0; // 연결된 클라이언트와 통신하는 데 사용할 통신 소켓의 소켓 핸들을 저장한다.
	char szBuffer[128] = { 0 }; // 클라이언트에서 받거나 클라이언트로 보내는 데이터를 저장한다.
	int nReceive = 0; // 클라이언트에서 받은 바이트 수를 저장한다. 일반적으로 클라이언트로부터 데이터를 수신하는 데 사용되는 recv 함수의 반환 값을 확인하는 데 사용된다.

	//4.1 클라이언트 연결을 받아들이고 새로운 소켓 생성(개방)
	while ((hClient = ::accept(hSocket, // accept()에서 클라이언트와 통신할 통신소켓을 반환한다.
		(SOCKADDR*)&clientaddr, // 클라이언트의 주소 정보는 clientaddr 구조에 저장된다.
		&nAddrLen)) != INVALID_SOCKET) 
	{
		puts("새 클라이언트가 연결되었습니다."); fflush(stdout);
		//4.2 클라이언트로부터 문자열을 수신
		while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0) // recv 함수가 0보다 큰 값을 반환하면 클라이언트에서 보낸 데이터를 수신했음을 나타낸다.
																				// Tcp Layer에 있는 버퍼에서 szBuffer 배열로 전송받은 데이터를 복사한다.
		{                                                                       // 0을 반환한 경우, 클라이언트가 연결을 끊었다고 간주한다.
			//4.3 수신한 문자열을 그대로 반향 전송
			::send(hClient, szBuffer, sizeof(szBuffer), 0); // 에코 응답으로 클라이언트에 데이터를 다시 보낸다.
			puts(szBuffer); fflush(stdout); // 수신했던 데이터를 콘솔에도 출력한다.
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		//4.3 클라이언트가 연결을 종료
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hClient);
		puts("클라이언트 연결이 끊겼습니다."); fflush(stdout);
	}

	//5. 리슨 소켓 닫기
	::closesocket(hSocket);

	//※윈속 해제
	::WSACleanup();
	return 0;
}
