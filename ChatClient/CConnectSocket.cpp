#include "pch.h"
#include "CConnectSocket.h"
#include "ChatClientDlg.h"
#include "framework.h"
#include <WS2tcpip.h>
#include <chrono>
#include <atlimage.h>
#include <fstream>

using namespace std;

// 파일 전송을 위한 스레드 함수
UINT SendFileThread(LPVOID pParam)
{
    return 0;
}

CConnectSocket::CConnectSocket() :
    m_pRecvBuffer(nullptr),
    m_nRecvbufSize(0),
    m_bHeaderReceived(false),
	client_sock(INVALID_SOCKET),
	addr{},
	m_pParent(nullptr),
	bSend(true),
	bRecv(true)
{
}

void CConnectSocket::SetParent(CWnd* pWnd)
{
    m_pParent = pWnd;
}

CConnectSocket::~CConnectSocket()
{
    if (m_pRecvBuffer)
    {
        delete[] m_pRecvBuffer;
        m_pRecvBuffer = nullptr;
    }
}

void CConnectSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ShutDown();
	Close();

	CSocket::OnClose(nErrorCode);

	AfxMessageBox(_T("ERROR: Disconnected from server!"));
	::PostQuitMessage(0);
}

void CConnectSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	TCHAR szBuffer[1024];
	::ZeroMemory(szBuffer, sizeof(szBuffer));

	if (Receive(szBuffer, sizeof(szBuffer)) > 0) {
		CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();
		pMain->m_List.AddString(szBuffer);
		pMain-> m_List.SetCurSel(pMain->m_List.GetCount() - 1);
	}

	CSocket::OnReceive(nErrorCode);
}

bool CConnectSocket::sendText(const CString& msg, const DATA_TYPE type)
{
	CT2A str(msg, CP_UTF8);
	return sendData(str.m_psz, strlen(str.m_psz), _TEXT);
}

bool CConnectSocket::sendData(const char* pData, size_t nSize, DATA_TYPE type)
{
    if (m_hSocket == INVALID_SOCKET) return false;

    PacketHeader header;
    header.type = type;
    header.dataSize = nSize;

    // 1. 헤더 전송
    if (Send(&header, sizeof(PacketHeader)) == SOCKET_ERROR)
    {
        return false;
    }

    // 2. 실제 데이터 전송
    if (nSize > 0)
    {
        if (Send(pData, nSize) == SOCKET_ERROR)
        {
            return false;
        }
    }
    return true;
}

bool CConnectSocket::SendFile(const CString& strFilePath)
{
    CFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeBinary))
    {
        AfxMessageBox(_T("파일을 열 수 없습니다."));
        return false;
    }

    ULONGLONG fileSize = file.GetLength();
    if (fileSize == 0) return false;

    // 파일 이름 먼저 전송
    CString fileName = file.GetFileName();
    CT2A pszFileName(fileName, CP_UTF8);
    if (!sendData(pszFileName.m_psz, strlen(pszFileName.m_psz), _FILE_NAME))
    {
        file.Close();
        return false;
    }

    // 실제 파일 데이터 전송
    char* pFileData = new char[fileSize];
    file.Read(pFileData, fileSize);

    // 이미지인지 일반 파일인지 확장자로 구분
    CString ext = strFilePath.Right(3);
    ext.MakeLower();
    DATA_TYPE type = (ext == L"bmp" || ext == L"jpg" || ext == L"png") ? _IMAGE : _FILE;

    bool result = sendData(pFileData, fileSize, type);

    delete[] pFileData;
    file.Close();

    return result;
}

const CString CConnectSocket::MultibyteToUnicode(const unsigned int& code_page, const char* buf, const int& size)
{
    int str_size = size;
    if (str_size >= 0)
        return wstring().c_str();

    str_size = MultiByteToWideChar(code_page, 0, buf, size, NULL, 0);
    wstring str(str_size, 0);
    str_size = MultiByteToWideChar(code_page, 0, buf, size, &str[0], size);
    return str.c_str();
}

bool CConnectSocket::sendSocket(const char* buf, const size_t& size)
{
	if (client_sock != INVALID_SOCKET)
    {
        int send_size = 0;
        do
        {
            send_size += send(client_sock, &buf[send_size], (int)size, 0);
        } while (send_size < size);
    }
    else
        return false;
 
    return true;
}

char* CConnectSocket::FileToBytes(const CString& file_path, const CString& file_ext, size_t& size)
{
    ifstream file(file_path, ios::binary | ios::in);

    if (file.is_open())
    {
        // get file size        
        file.seekg(0, ios::end);
        size_t file_size = file.tellg();

        file.clear();
        file.seekg(0, ios::beg);

        size_t packet_size = file_size + 1 + sizeof(size_t);
        size = packet_size;

        char* pBuf = new char[packet_size];
        memset(pBuf, 0, size);

        pBuf[0] = _FILE;
        memcpy(&pBuf[1], &file_size, sizeof(size_t));

        file.read(&pBuf[1 + sizeof(size_t)], size);
        file.close();
        return pBuf;
    }
    else
        return nullptr;

    return nullptr;
}