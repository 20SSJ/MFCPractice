#include "pch.h"
#include "Server.h"
#include <chrono>
#include <ws2tcpip.h>
#include <fstream>

using namespace std;

// 스레드 함수 전방 선언
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

bool Server::startServer(LPCTSTR ip, int port)
{
	if (listen_sock == INVALID_SOCKET)
	{
		listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listen_sock == INVALID_SOCKET)
			return false;

		addr.sin_family = AF_INET;
		InetPton(AF_INET, ip, &(addr.sin_addr));
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

	if (listen_sock != INVALID_SOCKET)
	{
		shutdown(listen_sock, SD_BOTH);
		closesocket(listen_sock);
		listen_sock = INVALID_SOCKET;
	}

	if (m_pListenThread)
	{
		if (m_pListenThread->joinable())
			m_pListenThread->join();
		delete m_pListenThread;
		m_pListenThread = nullptr;
	}

	m_mutex.lock();
	for (SOCKET sock : m_clientSock)
	{
		shutdown(sock, SD_BOTH);
		closesocket(sock);
	}
	m_clientSock.clear();
	m_mutex.unlock();

	for (thread* pThread : m_clientThread)
	{
		if (pThread->joinable())
			pThread->join();
		delete pThread;
	}
	m_clientThread.clear();
}

// Server.cpp 파일

wstring Server::getMyip()
{
	char host[MAX_PATH] = { 0, };
	if (gethostname(host, sizeof(host)) == SOCKET_ERROR)
		return wstring();

	addrinfo hint = {}, * pResult = nullptr;
	hint.ai_family = AF_INET;

	if (getaddrinfo(host, "7000", &hint, &pResult) != 0)
		return wstring();

	wstring ip; // 최종적으로 반환될 wstring
	if (pResult) {
		sockaddr_in* pIpv4 = (struct sockaddr_in*)pResult->ai_addr;

		// IP 주소를 멀티바이트(char) 버퍼에 저장합니다.
		char ip_str[INET_ADDRSTRLEN] = { 0 };
		InetNtop(AF_INET, &pIpv4->sin_addr, ip_str, INET_ADDRSTRLEN);

		int str_len = MultiByteToWideChar(CP_ACP, 0, ip_str, -1, NULL, 0);
		if (str_len > 0) {
			ip.resize(str_len - 1); // null 문자를 제외한 길이로 wstring 크기 조절
			MultiByteToWideChar(CP_ACP, 0, ip_str, -1, &ip[0], str_len);
		}
	}

	freeaddrinfo(pResult);
	return ip;
}

bool Server::removeClient(const SOCKET& sock, const SOCKADDR_IN& addr)
{
	m_mutex.lock();
	for (auto it = m_clientSock.begin(); it != m_clientSock.end(); ++it)
	{
		if (*it == sock)
		{
			m_clientSock.erase(it);
			UINT_PTR wp;
			LONG_PTR lp;
			setAddrFromSocket(addr, wp, lp);
			if (m_pParent) m_pParent->PostMessage(UM_DISCONNECT_CLIENT, wp, lp);
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

void Server::sendText(const CString& msg, const DATA_TYPE& type)
{
	if (msg.IsEmpty()) return;

	string utf8_str = CT2A(msg, CP_UTF8);

	size_t data_size = utf8_str.length();
	size_t packet_size = 1 + sizeof(size_t) + data_size;

	char* buf = new char[packet_size];
	buf[0] = type;
	memcpy(&buf[1], &data_size, sizeof(size_t));
	memcpy(&buf[1 + sizeof(size_t)], utf8_str.c_str(), data_size);

	broadcast(buf, packet_size);
	delete[] buf;
}

void Server::recvFinished(const DATA_TYPE& type, const char* data_buf, const size_t& data_size, const SOCKET& client_sock, const char* nick, const size_t& nick_size)
{
	if (!m_pParent) return;

	switch (type)
	{
	case _NICK:
		m_pParent->SendMessage(UM_RECV_NICK, (WPARAM)data_buf, (LPARAM)data_size);
		break;
	case _TEXT:
	{
		size_t packet_size = 1 + sizeof(size_t) + data_size;
		char* full_packet = new char[packet_size];
		full_packet[0] = type;
		memcpy(&full_packet[1], &data_size, sizeof(size_t));
		memcpy(&full_packet[1 + sizeof(size_t)], data_buf, data_size);
		broadcast(full_packet, packet_size);
		delete[] full_packet;

		m_pParent->SendMessage(UM_RECV_TEXT, (WPARAM)data_buf, (LPARAM)data_size);
		break;
	}
	case _IMAGE_NAME:
	case _FILE_NAME:
	{
		wstring fileName = MultibyteToUnicode(CP_UTF8, data_buf, (int)data_size);
		m_fileMutex.lock();
		m_mapFileNames[client_sock] = fileName;
		m_fileMutex.unlock();

		wstring wNick = MultibyteToUnicode(CP_UTF8, nick, (int)nick_size);
		wstring msg = L"[" + wNick + L"] 님으로부터 파일 수신 시작: " + fileName;
		string utf8_msg = UnicodeToMultibyte(CP_UTF8, msg);
		m_pParent->SendMessage(UM_RECV_TEXT, (WPARAM)utf8_msg.c_str(), (LPARAM)utf8_msg.length());
		break;
	}
	case _IMAGE:
	case _FILE:
	{
		m_fileMutex.lock();
		auto it = m_mapFileNames.find(client_sock);
		if (it != m_mapFileNames.end())
		{
			wstring fileName = it->second;
			m_mapFileNames.erase(it);
			m_fileMutex.unlock();

			wstring savePath = L"ReceivedFiles\\";
			CreateDirectoryW(savePath.c_str(), NULL);
			savePath += fileName;

			ofstream file(savePath, ios::binary);
			if (file.is_open())
			{
				file.write(data_buf, data_size);
				file.close();

				wstring msg = L"파일 수신 완료: " + fileName;
				string utf8_msg = UnicodeToMultibyte(CP_UTF8, msg);
				m_pParent->SendMessage(UM_RECV_TEXT, (WPARAM)utf8_msg.c_str(), (LPARAM)utf8_msg.length());
			}
		}
		else {
			m_fileMutex.unlock();
		}
		break;
	}
	}
}

bool Server::sendSocket(const char* buf, const size_t& size)
{
	broadcast(buf, size);
	return true;
}

void Server::broadcastNick(const char* buf, const size_t& size)
{
}

void Server::broadcast(const char* buf, const size_t& size)
{
	m_mutex.lock();
	for (SOCKET sock : m_clientSock)
	{
		size_t total_sent = 0;
		while (total_sent < size)
		{
			int sent = send(sock, &buf[total_sent], (int)(size - total_sent), 0);
			if (sent == SOCKET_ERROR) break;
			total_sent += sent;
		}
	}
	m_mutex.unlock();
}

const string Server::UnicodeToMultibyte(const unsigned int& code_page, const std::wstring& strWide)
{
	if (strWide.empty()) return string();
	int size = WideCharToMultiByte(code_page, 0, strWide.c_str(), -1, NULL, 0, NULL, NULL);
	string str(size - 1, 0);
	WideCharToMultiByte(code_page, 0, strWide.c_str(), -1, &str[0], size, NULL, NULL);
	return str;
}

const wstring Server::MultibyteToUnicode(const unsigned int& code_page, const char* buf, const int& size)
{
	if (size <= 0) return wstring();
	int str_size = MultiByteToWideChar(code_page, 0, buf, size, NULL, 0);
	wstring str(str_size, 0);
	MultiByteToWideChar(code_page, 0, buf, size, &str[0], str_size);
	return str;
}

// === 스레드 함수 ===
unsigned int listenThread(LPVOID p, const SOCKET& sock)
{
	Server* pS = reinterpret_cast<Server*>(p);
	while (pS->bListen)
	{
		if (listen(sock, SOMAXCONN) == SOCKET_ERROR) break;

		SOCKADDR_IN addr = {};
		int size = sizeof(addr);
		SOCKET client = accept(sock, (SOCKADDR*)&addr, &size);

		if (client != INVALID_SOCKET)
		{
			pS->addClient(client);
			thread* pT = new thread(&clientThread, p, client, addr);
			pS->addThread(pT);

			UINT_PTR wp;
			LONG_PTR lp;
			pS->setAddrFromSocket(addr, wp, lp);
			if (pS->m_pParent) pS->m_pParent->PostMessage(UM_CONNECT_CLIENT, wp, lp);
		}
		else break;
	}
	return 0;
}

unsigned int clientThread(LPVOID p, SOCKET sock, SOCKADDR_IN addr)
{
	Server* pS = reinterpret_cast<Server*>(p);
	char* pDataBuf = nullptr;
	char nick[128] = { 0 };
	size_t nick_size = 0;

	while (pS->bClient)
	{
		bool bDisconnect = false;

		char header[1 + sizeof(size_t)];
		int received = 0;
		while (received < sizeof(header)) {
			int result = recv(sock, header + received, sizeof(header) - received, 0);
			if (result <= 0) { bDisconnect = true; break; }
			received += result;
		}
		if (bDisconnect) break;

		DATA_TYPE type = static_cast<DATA_TYPE>(header[0]);
		size_t data_size = 0;
		memcpy(&data_size, &header[1], sizeof(size_t));

		if (data_size > 0) {
			pDataBuf = new char[data_size];
			received = 0;
			while (received < data_size) {
				int result = recv(sock, pDataBuf + received, (int)(data_size - received), 0);
				if (result <= 0) { bDisconnect = true; break; }
				received += result;
			}
			if (bDisconnect) {
				delete[] pDataBuf;
				pDataBuf = nullptr;
				break;
			}
		}

		if (nick_size == 0 && type == _NICK) {
			if (pDataBuf) memcpy(nick, pDataBuf, data_size);
			nick_size = data_size;
		}
		pS->recvFinished(type, pDataBuf, data_size, sock, nick, nick_size);

		if (pDataBuf) {
			delete[] pDataBuf;
			pDataBuf = nullptr;
		}
	}

	pS->removeClient(sock, addr);
	closesocket(sock);
	return 0;
}