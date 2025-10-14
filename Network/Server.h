#pragma once
#pragma comment(lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <map>

using namespace std;

const int MAX_BUF = 4096;

enum DATA_TYPE { _NICK, _TEXT, _IMAGE_NAME, _IMAGE, _FILE, _FILE_NAME, _UNKNOWN };

const unsigned int UM_CONNECT_CLIENT = WM_USER + 1;
const unsigned int UM_DISCONNECT_CLIENT = WM_USER + 2;
const unsigned int UM_RECV_NICK = WM_USER + 3;
const unsigned int UM_RECV_TEXT = WM_USER + 4;
const unsigned int UM_RECV_IMAGE = WM_USER + 5;
const unsigned int UM_RECV_FILE = WM_USER + 6;

class Server : public CWnd
{
public:
	Server(CWnd* pParent);
	~Server();

public:
	bool startServer(LPCTSTR ip, int port = 7000);
	void endServer();

	static wstring getMyip();
	inline void addClient(const SOCKET& sock) { m_clientSock.push_back(sock); }
	inline void addThread(thread* const pT) { m_clientThread.push_back(pT); }
	bool removeClient(const SOCKET& sock, const SOCKADDR_IN& addr);
	void setAddrFromSocket(const SOCKADDR_IN& addr, UINT_PTR& wp, LONG_PTR& lp);

	void sendText(const CString& msg, const DATA_TYPE& type = _TEXT);
	void recvFinished(const DATA_TYPE& type, const char* data_buf, const size_t& data_size, const SOCKET& client_sock, const char* nick, const size_t& nick_size);

	static const string UnicodeToMultibyte(const unsigned int& code_page, const wstring& strWide);
	static const wstring MultibyteToUnicode(const unsigned int& code_page, const char* pBuf, const int& size);

private:
	void broadcastNick(const char* buf, const size_t& size);
	void broadcast(const char* buf, const size_t& size);
	bool sendSocket(const char* buf, const size_t& size);

private:
	SOCKET listen_sock;
	SOCKADDR_IN  addr;

	map<SOCKET, wstring> m_mapFileNames;
	mutex m_fileMutex;
	thread* m_pListenThread;
	vector<thread*> m_clientThread;
	vector<SOCKET> m_clientSock;

	mutex m_mutex;

public:
	bool bListen, bClient;
	CWnd* m_pParent;
};
