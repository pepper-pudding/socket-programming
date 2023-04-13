// SockReuse.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	//�� ���ε� ���� IP�ּҿ� ��Ʈ�� �����ϵ��� ���� �ɼ��� �����Ѵ�.
	 
	// SO_REUSEADDR �̷��� ���� �ɼ��� �ָ� IP �ּҿ� PORT ��ȣ�� ���Ƶ�
	// ������ �߻����� �ʰ� bind()�� �̷������.
	// ������ ���� ���, ���� IP �ּҿ� PORT ��ȣ�� ����� ���� ������ ����ȴ�.

	// SO_REUSEADDR �ɼ�(IP �ּҿ� PORT ��ȣ�� �����ϴ� �ɼ�)�� ����ϴ� ���
	// 1. ������ �������� ��Ȳ�� �����ϱ� ���ؼ�.
	// 2. ������ �ݴ� �ʿ����� TIME_WAIT ���¸� �ʿ������� ��ġ�� �Ǵµ�,
	// �̴� ª�Դ� �� �ʿ��� ��Դ� �� �о� �ɸ���.
	// ���� ������ �׾ ������ �ݴ� ��쿡 �̷��� TIME_WAIT ���¿� ������ ��Ȳ�� ���ܼ�
	// ���� ������� �� �Ǵ� ��Ȳ�� �����ϱ� ���� SO_REUSEADDR �ɼ��� ����� ���� �ִ�.


	BOOL bOption = TRUE;
	if (::setsockopt(hSocket, SOL_SOCKET,
		SO_REUSEADDR, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR)
	{
		puts("ERROR: ���� �ɼ��� ������ �� �����ϴ�.");
		return 0;
	}

	//���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	//	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// SO_REUSEADDR �ɼ�(IP �ּҿ� PORT ��ȣ�� �����ϴ� �ɼ�)�� ����ϴ� ���,
	// �ƹ� IP �ּҳ� �� ������ �� �ְ� �ϴ� INADDR_ANY�� ����ϸ� �� �ȴ�.
	// �ֳ��ϸ� �߸��� ������ ����Ǵ� ��찡 ���� �� �ֱ� �����̴�.

	// ���� �� �� ī��(NIC)�� ���� ���� ��� IP �ּҰ� 2���� ���� �ִ�.
	// �ּҰ� 1.1.1.1, 2.2.2.2 �̷��� �ְ� ���� ���μ��� A, ���� ���μ��� B �̷��� ����� ��
	// ���Ͽ� SO_REUSEADDR �ɼ��� �����ϰ� INADDR_ANY, �ƹ� IP �ּҸ� �� �޴� ���,
	// A ������ �׾��� �� � �ּҷ� ������ ��Ʈ ��ȣ�� ������ �ޱ� ������
	// B ������ �����Ͱ� ���۵� ���� �ִ�.

	svraddr.sin_addr.S_un.S_addr = inet_addr("192.168.77.2");
	if (::bind(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}

	puts("Echo ������ �����մϴ�.");

	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	while ((hClient = ::accept(hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	{
		puts("�� Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");
		while ((nReceive = ::recv(hClient,
			szBuffer, sizeof(szBuffer), 0)) > 0)
		{
			::send(hClient, szBuffer, sizeof(szBuffer), 0);
			puts(szBuffer);
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		::closesocket(hClient);
		puts("Ŭ���̾�Ʈ ������ ������ϴ�.");
	}

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}


