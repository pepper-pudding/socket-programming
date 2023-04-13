// SocketBuffer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	// ������ ������ File�̴�.
	// File�� ����� ����� ���� ������ ������ ���������� ���۸� ����Ѵ�. (Buffered File I/O)
	// ���μ����� �������� �����͸� ���(Send) / �Է�(Read)�Ѵ�.
	// ������ �Է� ����, ��� ���۸� ������ �ִ�.
	// ���� ����ڷκ��� Ű �Է��� ���� "Hello"��� ���ڿ��� �Է¹ް� �̸� char szBuffer[128]; �迭�� �����ߴٰ� ����.
	// �׸��� send() �Լ��� ���ڷ� szBuffer�� �Ѱܼ� ������ ��� ���ۿ� "Hello" ���ڿ��� �Է��ߴٰ� ����.
	// ������ ��� ���۸� TCP ������ �����ϸ鼭 Segmentation�� �Ͼ��,
	// IP �������� �� Segment�� Packet���� ���Ѵ�. �̶� ��Ŷ�� �ִ� ���� ����(MTU)�� 1500����Ʈ�̴�.
	// ��Ŷ�� ���� �ӵ��� ��Ÿ�� �� bps(bit per second), pps(packet per seconde)��� �� ���� ��ġ�� ����Ѵ�.
	// �ٵ� TCP/IP ���������� ����ϴ� ��Ŷ�� ��� ����� ũ�Ⱑ 40����Ʈ ������ �ȴ�.
	// �ܿ� "Hello"��� ª�� ���ڿ��� �������ڰ� 40����Ʈ�� �߰��� ������ �ϴ� ���̴�.
	// bps�� ���� �� ������, pps�� ���� �� ���� �ʴ�.
	// �̷� ��ȿ������ ȿ�������� �ٲٴ� ����� ������ ��� ���۸� �ִ��� ä���� ������ ���̴�.
	// �̸� Nagle �˰����̶�� �Ѵ�.
	// ������ �Է� ���۵� ���������̴�.
	// ���� �ʰ� Ŀ�� ���� ����Ī Ƚ���� ���� �� �ִ�.
	// �׷��� ���ۿ� �� ������ ��ٸ��µ�, �󸶳� �������� ��ٷ��� �ұ�?
	// �̰� ǥ���� ������ ���� �ƴϱ� ������ TCP�� ����ü�� ������ ��� �����̴�. �������� ��쿡�� Compound TCP�ΰ� �׷��ٰ� �Ѵ�.
	// �켱 ������ ũ�⸦ ���� �˰� �Ѿ��.


	//������ '�۽�' ������ ũ�⸦ Ȯ���ϰ� ����Ѵ�.
	int nBufSize = 0, nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Send buffer size: %d\n", nBufSize); // 65536, �� 64k

	//������ '����' ������ ũ�⸦ Ȯ���ϰ� ����Ѵ�.
	nBufSize = 0;
	nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Receive buffer size: %d\n", nBufSize); // 65538, �� 64k

	// ����� send()�� recv()�� 1:1�� ���ε��� �ʴ´�.

	//������ �ݰ� ����.
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
