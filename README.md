# echo-client-server

- BoB 10th 3차 공통 교육 과제 #2
- echo-client, echo-server 프로그램 제작하기

## 실행
**1. echo-server**
```
syntax : echo-server <port> [-e[-b]]
sample : echo-server 1234 -e -b
```

**2. echo-client**
```
syntax : echo-client <ip> <port>
sample : echo-client 192.168.10.2 1234
```

## 기능
1. client는 사용자로부터 메세지를 입력받아 server에게 메세지를 전달한다.
2. server는 받은 메세지를 화면에 출력하고 `-e`(echo) 옵션이 주어진 경우 client에게 그대로 보낸다.
3. server는 `-b`(broadcast) 옵션이 주어진 경우 접속되어 있는 모든 client에게 메세지를 보낸다.   
4. client는 server로부터 메세지를 받으면 화면에 출력한다.

### 상세
- `echo-client`는 `echo-server`에게 TCP 접속
- server는 여러개 client의 접속 요청 및 데이터 처리가 
- broadcaset 기능은 vector를 활용하여 구현
