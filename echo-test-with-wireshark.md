# 에코 클라이언트/서버 테스트 및 Wireshark로 확인

## Wireshark 셋팅

Wireshark 설치 후 Adapter for loopback traffic capture를 선택한다.
로컬 호스트인 127.0.0.1, 즉 loopback을 통해서 에코 클라이언트와 에코 서버를 연결하고 패킷을 주고 받는 테스트를 할 것이기 때문이다.

에코 클라이언트와 에코 서버끼리 주고 받는 패킷만 보기 위해서 아래 필터를 걸어준다.
```
tcp.port == 25000
```

## 에코 서버 실행

에코 서버를 실행하고 명령 프롬프트 창에서 아래 명령어를 입력해보면
포트 25000번에서 실행되고 있는 에코 서버 프로세스를 확인할 수 있다.

LISTENING 상태로 되어있다.

```
netstat -ano | find "25000"
```

![image](https://user-images.githubusercontent.com/106955460/229267063-7af02337-8fd2-4e9b-bc93-c736fa70d494.png)

## 에코 클라이언트 실행

에코 클라이언트를 실행하자마자 에코 서버에 접속하면서 3 way handshacking을 시도한다.
Wireshark에서 로컬 loopback에서 통신이 이루어진 것을 확인할 수 있다.

Source Port : 57233으로 연결했다고 나오는데, netstat를 활용해서 진짜인지 확인해보자.

![image](https://user-images.githubusercontent.com/106955460/229267338-9cf09715-5f3c-40e1-8b91-670ee2b9e2f7.png)

Source Port : 57233으로 잘 연결된 것을 확인할 수 있다.
ESTABLISHED 상태로 되어있다.

![image](https://user-images.githubusercontent.com/106955460/229267321-e740cd05-9988-4cd2-982e-9f531e6f825d.png)

## 에코 클라이언트/서버 테스트

### 에코 클라이언트에서 문자열 전송

![image](https://user-images.githubusercontent.com/106955460/229273013-d9792690-ae4f-4fa6-8b17-91e69e28bf79.png)

### 에코 서버에서 문자열 반향 전송

![image](https://user-images.githubusercontent.com/106955460/229272930-23ade9d2-b8ba-47a9-a32a-89a7d6dbb6be.png)

### Wireshark

![image](https://user-images.githubusercontent.com/106955460/229273053-7f665494-406f-41e1-9787-8602dce1a5a3.png)

## 에코 클라이언트/서버 테스트 종료

### 에코 클라이언트 접속 끊기

![image](https://user-images.githubusercontent.com/106955460/229273093-cbb7f992-7282-4a76-8fbb-bdb487ad1276.png)

### 에코 서버

![image](https://user-images.githubusercontent.com/106955460/229273119-c0bbcee3-0ecd-4295-96fd-7c01d0f3f0e2.png)

### Wireshark

* FIN, ACK
* ACK
* FIN, ACK
* ACK
* 4 way handshacking을 주고 받으면서 연결이 끊기는 것을 확인할 수 있다.

![image](https://user-images.githubusercontent.com/106955460/229273194-773c839c-31e3-40fe-b987-a013f45a6d7d.png)


