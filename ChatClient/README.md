# ChatClient

이 프로젝트는 MFC 기반의 TCP 클라이언트 예제입니다. 사용자가 입력한 문자열을 서버로 전송하고, 서버가 보낸 메시지를 화면에 표시하는 구조로 되어 있습니다.

## 1. 한 줄 요약
- 클라이언트는 서버에 연결된 후 메시지를 보내고, 서버로부터 받은 내용을 리스트에 출력합니다.
- 핵심 클래스는 ChatClientDlg와 CConnectSocket입니다.
- 실제 통신은 Windows Socket(CSocket) 기반으로 이루어집니다.

## 2. 실행 흐름
1. 클라이언트 실행
2. ChatClientDlg의 OnInitDialog()에서 소켓을 생성하고 서버에 접속합니다.
3. 사용자가 입력한 메시지를 보내면 CConnectSocket가 서버로 전송합니다.
4. 서버가 메시지를 보내오면 OnReceive()에서 수신하여 UI에 표시합니다.

## 3. 통신 구조
```text
사용자 입력
   ↓
ChatClientDlg
   ↓
CConnectSocket::Send()
   ↓
TCP Socket
   ↓
ChatServer
   ↓
CClientSocket::OnReceive()
   ↓
서버 UI/다른 클라이언트로 전달
```

## 4. 주요 역할
### ChatClientDlg
- 화면(UI) 담당
- 사용자가 입력한 메시지를 확인하고, 전송 버튼 이벤트를 처리합니다.
- 프로그램이 시작될 때 서버 연결도 초기화합니다.

### CConnectSocket
- 실제 네트워크 통신 담당
- 서버와의 연결, 데이터 전송, 데이터 수신 이벤트를 처리합니다.
- 현재 채팅 예제에서는 문자열을 직접 전송하는 방식으로 동작합니다.

## 5. 코드 관점의 흐름
- OnInitDialog()
  - `m_Socket.Create()`로 소켓을 생성합니다.
  - `Connect("127.0.0.1", 21000)`으로 서버에 접속합니다.
- OnBnClickedButtonSend()
  - 입력한 문자열을 가져와 `Send()`로 전송합니다.
- OnReceive()
  - 서버가 보낸 데이터를 받아서 리스트 컨트롤에 추가합니다.

## 6. 참고
- 현재 예제는 기본 채팅 기능 중심입니다.
- 코드에는 파일/이미지 전송을 위한 패킷 구조 관련 함수도 포함되어 있어, 확장 가능성이 있습니다.
- 서버가 실행 중일 때만 정상적으로 통신할 수 있습니다.
