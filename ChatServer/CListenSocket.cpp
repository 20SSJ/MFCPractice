#include "pch.h"
#include "CListenSocket.h"
#include "CClientSocket.h"

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

inline void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CClientSocket* pClient = new CClientSocket;
	if (Accept(*pClient)) {
		pClient->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(pClient);
	}
	else {
		delete pClient;
		AfxMessageBox(_T("ERROR: Failed to accept new client!"));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}


void CListenSocket::CloseClientSocket(CSocket* pClient)
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);
	CClientSocket* pClient = NULL;

	// TODO: 여기에 구현 코드 추가.
	if (pClient != NULL) {
		pClient->ShutDown();
		pClient->Close();
	}
	m_ptrClientSocketList.RemoveAt(pos);
	delete pClient;
}

void CListenSocket::SendChatDataAll(TCHAR* pszMessage)
{
	// TODO: 여기에 구현 코드 추가.
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CCLientSocket* pClient = NULL;
}
