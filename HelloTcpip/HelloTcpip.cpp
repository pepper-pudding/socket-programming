// HelloTcpip.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//������ �ʱ�ȭ
	// Windows �÷������� ��Ʈ��ũ ���α׷��ֿ� ���Ǵ� Winsock ���̺귯���� �ʱ�ȭ�Ѵ�.

	WSADATA wsa = { 0 };
	// WSAStartup �Լ��� �� ���� �μ��� ȣ��ȴ�.
	// ù ��° �μ��� MAKEWORD(2, 2)�� ���� ���α׷����� ����� ������ ����Ǵ� Winsock�� ����(�� ��� ���� 2.2)�� �����Ѵ�.
	// �� ��° �μ��� Winsock ������ ���� ������ �޴� �� ���Ǵ� WSADATA ������ ���� �������̴�.
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//1. ���Ӵ�� ���� ����
	// Ŭ���̾�Ʈ�� ���� ��û�� ������ ������ �����.
	// Ŭ���̾�Ʈ�� ����� �� ����ϴ� ���ϰ��� ������ �����̴�.
	// ������ �޴� ��������(hSocket) + Ŭ���̾�Ʈ�� ����� ��ż���(hClient)

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0); // ������ IPv4 �����̸�,  TCP ���������� ����Ѵ�.
	// ù ��° �μ��� L3�� ��Ÿ����. AF_INET = IPv4, AF_INET6 = IPv6                                                    
	// �� ��° �μ��� L4�� ��Ÿ����. SOCK_STREAM = TCP, SOCK_DGRAM = UDP
	// �� ��° �μ��� �־��� ���� ������ ���� �⺻ ���������� �����ϴ� 0���� �����ȴ�.

	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	//2. ��Ʈ ���ε�
	// ������ Ư�� ��Ʈ ��ȣ �� IP �ּҿ� ���ε��Ѵ�.

	SOCKADDR_IN	svraddr = { 0 }; // ���� �ּҿ� ���� ������ ���Ե� SOCKADDR_IN ������ �ʱ�ȭ�Ѵ�.
	svraddr.sin_family = AF_INET; // ������ IPv4 �������� ��Ÿ����.
	svraddr.sin_port = htons(25000); // ��Ʈ ��ȣ : 25000. ��Ʈ ��ȣ�� ��Ʈ��ũ ����Ʈ ������ ��ȯ�ϱ� ���� htons �Լ��� ����Ѵ�.
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ������ �ý��ۿ��� ��� ������ ��� ��Ʈ��ũ �������̽��� ���ε��� �� �ִ�.
	if (::bind(hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) // bind �Լ��� ���� �ڵ�, sockaddr ������ ���� ������, sockaddr ������ ũ�⸦ �μ��� �޴´�.
	{
		puts("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	//3. ���Ӵ�� ���·� ��ȯ
	// ������ ������ ������ ������ �� �ֵ��� ������ LISTEN ���·� ��ȯ�Ѵ�.
	
	// listen �Լ��� �� ���� �μ��� ȣ��ȴ�.
	// ù ��° �μ��� 1.���� �����ǰ� 2.���� ���� �ּ� �� ��Ʈ ��ȣ�� ���ε��� ���� �ڵ��� hSocket�̴�.
	// �� ��° �μ� SOMAXCONN�� ���Ͽ� ����� �� �ִ� ���� ���� ������ �ִ� ���� ��Ÿ���� Ư�� ���̴�.
	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}

	//4. Ŭ���̾�Ʈ ���� ó�� �� ����
	// Ŭ���̾�Ʈ ������ ó���ϰ� ������ Ŭ���̾�Ʈ�� �ٽ� ������ �� ���Ǵ� ������ �����Ѵ�.

	SOCKADDR_IN clientaddr = { 0 }; // Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ ��ȣ�� ���� ������ �����Ѵ�.
	int nAddrLen = sizeof(clientaddr); // clientaddr ������ ũ�⸦ �����Ѵ�.
	SOCKET hClient = 0; // ����� Ŭ���̾�Ʈ�� ����ϴ� �� ����� ��� ������ ���� �ڵ��� �����Ѵ�.
	char szBuffer[128] = { 0 }; // Ŭ���̾�Ʈ���� �ްų� Ŭ���̾�Ʈ�� ������ �����͸� �����Ѵ�.
	int nReceive = 0; // Ŭ���̾�Ʈ���� ���� ����Ʈ ���� �����Ѵ�. �Ϲ������� Ŭ���̾�Ʈ�κ��� �����͸� �����ϴ� �� ���Ǵ� recv �Լ��� ��ȯ ���� Ȯ���ϴ� �� ���ȴ�.

	//4.1 Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	while ((hClient = ::accept(hSocket, // accept()���� Ŭ���̾�Ʈ�� ����� ��ż����� ��ȯ�Ѵ�.
		(SOCKADDR*)&clientaddr, // Ŭ���̾�Ʈ�� �ּ� ������ clientaddr ������ ����ȴ�.
		&nAddrLen)) != INVALID_SOCKET) 
	{
		puts("�� Ŭ���̾�Ʈ�� ����Ǿ����ϴ�."); fflush(stdout);
		//4.2 Ŭ���̾�Ʈ�κ��� ���ڿ��� ����
		while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0) // recv �Լ��� 0���� ū ���� ��ȯ�ϸ� Ŭ���̾�Ʈ���� ���� �����͸� ���������� ��Ÿ����.
																				// Tcp Layer�� �ִ� ���ۿ��� szBuffer �迭�� ���۹��� �����͸� �����Ѵ�.
		{                                                                       // 0�� ��ȯ�� ���, Ŭ���̾�Ʈ�� ������ �����ٰ� �����Ѵ�.
			//4.3 ������ ���ڿ��� �״�� ���� ����
			::send(hClient, szBuffer, sizeof(szBuffer), 0); // ���� �������� Ŭ���̾�Ʈ�� �����͸� �ٽ� ������.
			puts(szBuffer); fflush(stdout); // �����ߴ� �����͸� �ֿܼ��� ����Ѵ�.
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		//4.3 Ŭ���̾�Ʈ�� ������ ����
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hClient);
		puts("Ŭ���̾�Ʈ ������ ������ϴ�."); fflush(stdout);
	}

	//5. ���� ���� �ݱ�
	::closesocket(hSocket);

	//������ ����
	::WSACleanup();
	return 0;
}
