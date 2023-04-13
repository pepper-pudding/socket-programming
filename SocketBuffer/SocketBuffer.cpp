// SocketBuffer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//윈속 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	//소켓 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 소켓을 생성할 수 없습니다.");
		return 0;
	}

	// 소켓의 본질은 File이다.
	// File의 입출력 방식은 여러 가지가 있지만 보편적으로 버퍼를 사용한다. (Buffered File I/O)
	// 프로세스가 소켓으로 데이터를 출력(Send) / 입력(Read)한다.
	// 소켓은 입력 버퍼, 출력 버퍼를 가지고 있다.
	// 만약 사용자로부터 키 입력을 통해 "Hello"라는 문자열을 입력받고 이를 char szBuffer[128]; 배열에 저장했다고 하자.
	// 그리고 send() 함수의 인자로 szBuffer를 넘겨서 소켓의 출력 버퍼에 "Hello" 문자열을 입력했다고 하자.
	// 소켓의 출력 버퍼를 TCP 쪽으로 전달하면서 Segmentation이 일어나고,
	// IP 계층에서 이 Segment를 Packet으로 감싼다. 이때 패킷의 최대 전송 단위(MTU)는 1500바이트이다.
	// 패킷의 전송 속도를 나타낼 때 bps(bit per second), pps(packet per seconde)라는 두 가지 수치를 사용한다.
	// 근데 TCP/IP 프로토콜을 사용하는 패킷의 경우 헤더의 크기가 40바이트 정도가 된다.
	// 겨우 "Hello"라는 짧은 문자열을 전달하자고 40바이트를 추가로 보내야 하는 것이다.
	// bps가 높은 건 좋지만, pps가 높은 건 좋지 않다.
	// 이런 비효율성을 효율적으로 바꾸는 방법은 소켓의 출력 버퍼를 최대한 채워서 보내는 것이다.
	// 이를 Nagle 알고리즘이라고 한다.
	// 소켓의 입력 버퍼도 마찬가지이다.
	// 유저 쪽과 커널 쪽의 스위칭 횟수도 줄일 수 있다.
	// 그러면 버퍼에 찰 때까지 기다리는데, 얼마나 오랫동안 기다려야 할까?
	// 이게 표준이 정해진 것이 아니기 때문에 TCP의 구현체도 종류가 어려 가지이다. 윈도우의 경우에는 Compound TCP인가 그렇다고 한다.
	// 우선 버퍼의 크기를 먼저 알고 넘어가자.


	//소켓의 '송신' 버퍼의 크기를 확인하고 출력한다.
	int nBufSize = 0, nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Send buffer size: %d\n", nBufSize); // 65536, 약 64k

	//소켓의 '수신' 버퍼의 크기를 확인하고 출력한다.
	nBufSize = 0;
	nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Receive buffer size: %d\n", nBufSize); // 65538, 약 64k

	// 참고로 send()와 recv()는 1:1로 매핑되지 않는다.

	//소켓을 닫고 종료.
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
