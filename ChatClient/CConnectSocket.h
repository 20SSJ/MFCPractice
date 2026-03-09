#pragma once
#include <afxsock.h>

using namespace std;
class CConnectSocket :
    public CSocket
{
public:
    CConnectSocket();
    ~CConnectSocket();

    void SetParent(CWnd* pWnd);

    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);

    bool sendText(const CString& msg, const DATA_TYPE type);
    bool sendData(const char* pData, size_t nSize, DATA_TYPE type);
    bool SendFile(const CString& strFilePath);

    static const CString MultibyteToUnicode(const unsigned int& code_page, const char* buf, const int& size);

    bool sendSocket(const char* buf, const size_t& size);

    char* FileToBytes(const CString& file_path, const CString& file_ext, size_t& size);

    CString getNick() { return m_nick; }

    bool bRecv, bSend;
private:
    CWnd* m_pParent;

    char* m_pRecvBuffer;
    size_t m_nRecvbufSize;
    PacketHeader m_currentHeader;
    bool m_bHeaderReceived;

    SOCKET client_sock;
    SOCKADDR_IN addr;
    CString m_nick;
};
