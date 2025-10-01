#pragma once
#pragma comment(lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <string>
#include <mutex>

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
	bool startServer(std::wstring ip, int port = 7000);
	void endServer();

	static std::wstring getMyip();
	inline void addClient(const SOCKET& sock) { m_clientSock.push_back(sock); }
	inline void addThread(std::thread* const pT) { m_clientThread.push_back(pT); }
	bool removeClient(const SOCKET& sock, const SOCKADDR_IN& addr);
	void setAddrFromSocket(const SOCKADDR_IN& addr, UINT_PTR& wp, LONG_PTR& lp);

	void sendText(std::wstring msg, const DATA_TYPE& type = _TEXT);
	void recvFinished(const DATA_TYPE& type, const char* buf, const size_t& recv_size, const size_t& data_size, const char* nick, const size_t& nick_size);

	static const std::string UnicodeToMultibyte(const unsigned int& code_page, const std::wstring& strWide);
	static const std::wstring MultibyteToUnicode(const unsigned int& code_page, const char* pBuf, const int& size);

private:
	void broadcastNick(const char* buf, const size_t& size);
	void broadcast(const char* buf, const size_t& size);
	bool sendSocket(const char* buf, const size_t& size);

private:
	SOCKET listen_sock;
	SOCKADDR_IN  addr;

	std::thread* m_pListenThread;
	std::vector<std::thread*> m_clientThread;
	std::vector<SOCKET> m_clientSock;

	std::mutex m_mutex;

public:
	bool bListen, bClient;
	CWnd* m_pParent;
};
