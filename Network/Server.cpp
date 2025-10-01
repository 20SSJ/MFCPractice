#include "pch.h"
#include "Server.h"
#include <chrono>
#include <ws2tcpip.h>

using namespace std;

unsigned int listenThread(LPVOID p, const SOCKET& sock);
unsigned int clientThread(LPVOID p, SOCKET sock, SOCKADDR_IN addr);

Server::Server(CWnd* pParent)
	:listen_sock(INVALID_SOCKET), addr{}, m_pListenThread(nullptr), bListen(true), bClient(true), m_pParent(pParent)
{
}

Server::~Server()
{
	endServer();
}

bool Server::startServer(std::wstring ip, int port)
{
	if (listen_sock == INVALID_SOCKET)
	{
		listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listen_sock == INVALID_SOCKET)
			return false;

		addr.sin_family = AF_INET;
		//addr.sin_addr.s_addr = htonl(INADDR_ANY);		
		inet_pton(AF_INET, (PCSTR)ip.c_str(), &(addr.sin_addr));
		addr.sin_port = htons(port);

		int result = ::bind(listen_sock, (SOCKADDR*)&addr, (int)sizeof(addr));
		if (result == SOCKET_ERROR)
		{
			closesocket(listen_sock);
			return false;
		}

		if (m_pListenThread == nullptr)
		{
			bListen = bClient = true;
			m_pListenThread = new thread(&listenThread, this, listen_sock);
		}
	}

	return true;
}

void Server::endServer()
{
	bListen = bClient = false;

	// close listen socket
	if (listen_sock != INVALID_SOCKET)
	{
		closesocket(listen_sock);
		listen_sock = INVALID_SOCKET;
	}

	// stop listen thread
	if (m_pListenThread)
	{
		if (m_pListenThread->joinable())
			m_pListenThread->join();

		delete m_pListenThread;
		m_pListenThread = nullptr;
	}

	// close client socket
	m_mutex.lock();
	vector<SOCKET>::iterator sitr;
	for (sitr = m_clientSock.begin(); sitr != m_clientSock.end(); ++sitr)
	{
		closesocket(*sitr);
	}
	m_clientSock.clear();
	m_mutex.unlock();

	// stop client thread
	vector<thread*>::iterator itr;
	for (itr = m_clientThread.begin(); itr != m_clientThread.end(); ++itr)
	{
		if ((*itr)->joinable())
			(*itr)->join();

		delete (*itr);
	}
	m_clientThread.clear();
}

wstring Server::getMyip()
{
	char host[MAX_PATH] = { 0, };
	int result = gethostname(host, sizeof(host));

	if (result == SOCKET_ERROR)
		return wstring();

	// ip V4 만 가능
	//PHOSTENT pHostInfo;
	//pHostInfo = gethostbyname(host);
	//if (pHostInfo == nullptr)
	//	return string();

	//if (pHostInfo->h_addrtype == AF_INET) //ip V4
	//{
	//	in_addr addr;
	//	memset(&addr, 0, sizeof(addr));
	//	int i = 0;

	//	while (pHostInfo->h_addr_list[i] != 0)
	//	{
	//		addr.s_addr = *(u_long *)pHostInfo->h_addr_list[i++];
	//		inet_ntoa(addr);			
	//	}

	//}

	//// ip V6 대응
	char port_str[16] = { 0, };
	sprintf_s(port_str, "%d", 7000);
	addrinfo hint, * pResult;
	memset(&hint, 0, sizeof(hint));

	//result = getaddrinfo(host, port_str, &hint, &pResult);
	result = getaddrinfo(host, "7000", &hint, &pResult);

	if (result != 0)
		return wstring();

	addrinfo* ptr = nullptr;
	sockaddr_in* pIpv4;
	sockaddr_in6* pIpv6;

	wchar_t ip_str[46];
	DWORD ip_size = sizeof(ip_str);
	memset(&ip_str, 0, ip_size);

	wstring ip;

	for (ptr = pResult; ptr != NULL; ptr = ptr->ai_next)
	{
		switch (ptr->ai_family)
		{
		case AF_UNSPEC:
			break;
		case AF_INET:
			printf("AF_INET (IPv4)\n");
			pIpv4 = (struct sockaddr_in*)ptr->ai_addr;
			//inet_ntoa(pIpv4->sin_addr);
			InetNtop(AF_INET, &pIpv4->sin_addr, ip_str, ip_size);
			ip = ip_str;
			break;
		case AF_INET6:
			// the InetNtop function is available on Windows Vista and later
			pIpv6 = (struct sockaddr_in6*)ptr->ai_addr;
			InetNtop(AF_INET6, &pIpv6->sin6_addr, ip_str, ip_size);
			break;
		case AF_NETBIOS:
			break;
		default:
			break;
		}
	}

	return ip;
}

// 일부 클라이언트만 죽는 경우도 있으므로 벡터에서 삭제
bool Server::removeClient(const SOCKET& sock, const SOCKADDR_IN& addr)
{
	m_mutex.lock();
	vector<SOCKET>::iterator itr;
	for (itr = m_clientSock.begin(); itr != m_clientSock.end(); ++itr)
	{
		if ((*itr) == sock)
		{
			m_clientSock.erase(itr);

			UINT_PTR wp;
			LONG_PTR lp;
			setAddrFromSocket(addr, wp, lp);
			m_pParent->PostMessageW(UM_DISCONNECT_CLIENT, wp, lp);
			break;
		}
	}
	m_mutex.unlock();
	return true;
}

void Server::setAddrFromSocket(const SOCKADDR_IN& addr, UINT_PTR& wp, LONG_PTR& lp)
{
	wp = addr.sin_addr.s_addr;
	lp = htons(addr.sin_port);
}

unsigned int listenThread(LPVOID p, const SOCKET& sock)
{
	Server* pS = reinterpret_cast<Server*>(p);

	while (pS->bListen)
	{
		if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
		{
			pS->bListen = false;
			break;
		}

		SOCKET client = INVALID_SOCKET;
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		int size = sizeof(addr);

		client = accept(sock, (SOCKADDR*)&addr, &size);
		if (client != INVALID_SOCKET)
		{
			pS->addClient(client);
			thread* pT = new thread(&clientThread, p, client, addr);
			pS->addThread(pT);

			// Parent로 클라이언트 정보 전송
			UINT_PTR wp;
			LONG_PTR lp;
			pS->setAddrFromSocket(addr, wp, lp);
			pS->m_pParent->PostMessageW(UM_CONNECT_CLIENT, wp, lp);
		}
		else
		{
			break;
		}
	}
	closesocket(sock);
	return 0;
}

bool Server::sendSocket(const char* buf, const size_t& size)
{
	broadcast(buf, size);
	return false;
}

void Server::sendText(std::wstring msg, const DATA_TYPE& type)
{
	if (msg.empty())
		return;

	// utf-8 -> char
	string str = Server::UnicodeToMultibyte(CP_UTF8, msg);

	size_t txt_size = str.length();
	size_t packet_size = 1 + sizeof(size_t) + txt_size;

	char* buf = new char[packet_size];
	memset(buf, 0, packet_size);
	buf[0] = type;
	memcpy(&buf[1], &txt_size, sizeof(size_t));
	memcpy(&buf[1 + sizeof(size_t)], &str[0], txt_size);

	bool result = sendSocket(buf, packet_size);

	delete[] buf;
}

void Server::recvFinished(const DATA_TYPE& type, const char* buf, const size_t& recv_size, const size_t& data_size, const char* nick, const size_t& nick_size)
{
	if (m_pParent)
	{
		switch (type)
		{
		case _NICK:
			m_pParent->SendMessage(UM_RECV_NICK, (WPARAM)&buf[1 + sizeof(size_t)], data_size);
			break;
		case _TEXT:
			broadcast(buf, recv_size);
			m_pParent->SendMessage(UM_RECV_TEXT, (WPARAM)&buf[1 + sizeof(size_t)], data_size);
			break;
		case _IMAGE:
		case _FILE:
			broadcast(buf, recv_size);
			break;
		case _IMAGE_NAME:
		case _FILE_NAME:
			broadcast(buf, recv_size);
			m_pParent->SendMessage(UM_RECV_TEXT, (WPARAM)&buf[1 + sizeof(size_t)], data_size);
			break;
		}
	}
}

void Server::broadcastNick(const char* buf, const size_t& size)
{
	size_t data_size = 2 + size; // '[' ']'
	size_t packet_size = 1 + sizeof(size_t) + data_size;
	char* buf2 = new char[packet_size];
	memset(buf2, 0, data_size);
	buf2[0] = _TEXT;
	memcpy(&buf2[1], &data_size, sizeof(size_t));
	buf2[1 + sizeof(size_t)] = '[';
	memcpy(&buf2[1 + sizeof(size_t) + 1], buf, size);
	buf2[packet_size - 1] = ']';

	vector<SOCKET>::iterator itr;
	for (itr = m_clientSock.begin(); itr != m_clientSock.end(); ++itr)
	{
		size_t send_size = 0;
		do
		{
			send_size = send(*itr, &buf2[send_size], packet_size - send_size, 0);
		} while (send_size < packet_size);
	}
}

void Server::broadcast(const char* buf, const size_t& size)
{
	vector<SOCKET>::iterator itr;
	for (itr = m_clientSock.begin(); itr != m_clientSock.end(); ++itr)
	{
		if (size <= INT_MAX)
		{
			int send_size = 0;
			do
			{
				send_size += send(*itr, &buf[send_size], (int)size, 0);
			} while (send_size < size);
		}
		else
		{
			size_t total_size = size;
			size_t pos = 0;
			int send_size = 0;
			int sent_size = 0;
			do
			{
				if (total_size > INT_MAX)
					send_size = INT_MAX;
				else
					send_size = total_size;

				sent_size = send(*itr, &buf[pos], send_size, 0);

				if (send_size == SOCKET_ERROR)
				{
					int e = WSAGetLastError();
					break;
				}

				total_size -= sent_size;
				pos += sent_size;

			} while (total_size > 0);
		}
	}
}

const string Server::UnicodeToMultibyte(const unsigned int& code_page, const std::wstring& strWide)
{
	if (strWide.empty())
		return string().c_str();

	int size = WideCharToMultiByte(code_page, 0, strWide.c_str(), (int)strWide.size(), NULL, 0, NULL, NULL);
	string str(size, 0);
	size = WideCharToMultiByte(code_page, 0, strWide.c_str(), (int)strWide.size(), &str[0], size, NULL, NULL);
	return str;
}

const wstring Server::MultibyteToUnicode(const unsigned int& code_page, const char* buf, const int& size)
{
	int str_size = size;
	if (str_size <= 0)
		return wstring().c_str();

	str_size = MultiByteToWideChar(code_page, 0, buf, size, NULL, 0);
	wstring str(str_size, 0);
	str_size = MultiByteToWideChar(code_page, 0, buf, size, &str[0], size);
	return str.c_str();
}

unsigned int clientThread(LPVOID p, SOCKET sock, SOCKADDR_IN addr)
{
	Server* pS = reinterpret_cast<Server*>(p);

	char buf[MAX_BUF];
	char* pRecvBuf = nullptr;
	char nick[128];
	memset(nick, 0, sizeof(nick));
	size_t nick_size = 0;

	while (pS->bClient)
	{
		memset(buf, 0, MAX_BUF);
		size_t total_size = 0;
		int recv_size = 0;
		size_t data_size = 0;
		size_t pos = 0;
		DATA_TYPE type = _UNKNOWN;
		bool bDisconnect = false;

		do
		{
			recv_size = recv(sock, buf, MAX_BUF, 0);

			if (recv_size < 0)
			{
				bDisconnect = true;
				break;
			}
			else
			{
				total_size += recv_size;
				if (type == _UNKNOWN && recv_size >= 1 + sizeof(size_t))
				{
					type = static_cast<DATA_TYPE>(buf[0]);
					memcpy(&data_size, &buf[1], sizeof(size_t));

					if (pRecvBuf == nullptr)
					{
						size_t packet_size = 1 + sizeof(size_t) + data_size;
						pRecvBuf = new char[packet_size];
						memset(pRecvBuf, 0, packet_size);
					}
				}
				memcpy(&pRecvBuf[pos], buf, recv_size);
				pos += recv_size;
			}

		} while (total_size < data_size + 1 + sizeof(size_t));

		if (bDisconnect)
		{
			pS->removeClient(sock, addr);
			std::this_thread::sleep_for(100ms);
			break;
		}

		if (total_size == data_size + 1 + sizeof(size_t))
		{
			if (nick_size == 0 && type == _NICK)
			{
				memcpy(nick, &buf[1 + sizeof(size_t)], data_size);
				nick_size = data_size;
			}

			pS->recvFinished(type, pRecvBuf, total_size, data_size, nick, nick_size);
			if (pRecvBuf)
			{
				delete[] pRecvBuf;
				pRecvBuf = nullptr;
			}
		}
	}
	if (pRecvBuf)
	{
		delete[] pRecvBuf;
		pRecvBuf = nullptr;
	}
	closesocket(sock);
	sock = INVALID_SOCKET;
	return 0;
}