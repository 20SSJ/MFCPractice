#include "pch.h"
#include "CClientSocket.h"
#include "CListenSocket.h"

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnClose(nErrorCode);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
}

void CClientSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	// TODO: 여기에 구현 코드 추가.
	m_pListenSocket = pSocket;
}
