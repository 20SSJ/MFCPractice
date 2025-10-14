// NetworkDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Network.h"
#include "NetworkDlg.h"
#include "afxdialogex.h"

#include "Server.h"
#include <string>
#include <WS2tcpip.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNetworkDlg 대화 상자

CNetworkDlg::CNetworkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NETWORK_DIALOG, pParent), m_pServer(nullptr)
	, m_port(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CNetworkDlg::~CNetworkDlg()
{
	closeServer();
}

void CNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_ip);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port);
	DDV_MinMaxInt(pDX, m_port, 1024, 65535);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_LIST_MSG, m_listMsg);
}

BEGIN_MESSAGE_MAP(CNetworkDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS, &CNetworkDlg::OnIpnFieldchangedIpaddress)
	ON_BN_CLICKED(IDC_BTN_START, &CNetworkDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CNetworkDlg::OnBnClickedBtnStop)
	ON_MESSAGE(UM_CONNECT_CLIENT, &CNetworkDlg::OnConnectClient)
	ON_MESSAGE(UM_DISCONNECT_CLIENT, &CNetworkDlg::OnDisconnectClient)
	ON_MESSAGE(UM_RECV_NICK, &CNetworkDlg::OnRecevieNick)
	ON_MESSAGE(UM_RECV_TEXT, &CNetworkDlg::OnRecevieText)
END_MESSAGE_MAP()


// CNetworkDlg 메시지 처리기

BOOL CNetworkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CString ip(Server::getMyip().c_str());
	m_ip.SetWindowText(ip);
	m_port = 7000;
	UpdateData(false);

	GetDlgItem(IDC_BTN_STOP)->EnableWindow(false);
	initListClient();
	return TRUE;
}

void CNetworkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) { CAboutDlg dlgAbout; dlgAbout.DoModal(); }
	else { CDialogEx::OnSysCommand(nID, lParam); }
}

void CNetworkDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else { CDialogEx::OnPaint(); }
}

HCURSOR CNetworkDlg::OnQueryDragIcon() { return static_cast<HCURSOR>(m_hIcon); }
void CNetworkDlg::OnIpnFieldchangedIpaddress(NMHDR* pNMHDR, LRESULT* pResult) { *pResult = 0; }

void CNetworkDlg::OnBnClickedBtnStart()
{
	if (m_pServer == nullptr)
	{
		UpdateData(true);
		CString ip;
		m_ip.GetWindowText(ip);

		m_pServer = new Server(this);
		m_pServer->startServer(ip, m_port);

		GetDlgItem(IDC_BTN_START)->EnableWindow(false);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(true);
	}
}

void CNetworkDlg::OnBnClickedBtnStop()
{
	closeServer();
	GetDlgItem(IDC_BTN_START)->EnableWindow(true);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(false);
	m_list.DeleteAllItems();
}

void CNetworkDlg::closeServer()
{
	if (m_pServer)
	{
		m_pServer->endServer();
		delete m_pServer;
		m_pServer = nullptr;
	}
}

void CNetworkDlg::initListClient()
{
	CRect rect;
	m_list.GetClientRect(&rect);
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("닉네임"), LVCFMT_LEFT, (int)(rect.Width() * 0.2));
	m_list.InsertColumn(1, _T("IP"), LVCFMT_LEFT, (int)(rect.Width() * 0.2));
	m_list.InsertColumn(2, _T("PORT"), LVCFMT_LEFT, (int)(rect.Width() * 0.2));
	m_list.InsertColumn(3, _T("접속 시점"), LVCFMT_LEFT, (int)(rect.Width() * 0.3));
}

void CNetworkDlg::addListClient(const CString& ip, const CString& port)
{
	int cnt = m_list.GetItemCount();
	m_list.InsertItem(cnt, _T(""));
	m_list.SetItemText(cnt, 1, ip);
	m_list.SetItemText(cnt, 2, port);
	CString strtime = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	m_list.SetItemText(cnt, 3, strtime);
}

void CNetworkDlg::addListNick(const CString& nick)
{
	int cnt = m_list.GetItemCount();
	if (cnt > 0) {
		m_list.SetItemText(cnt - 1, 0, nick);
	}
}

void CNetworkDlg::addListText(const CString& msg)
{
	int index = m_listMsg.AddString(msg);
	m_listMsg.SetTopIndex(index);
}

LRESULT CNetworkDlg::OnConnectClient(WPARAM wp, LPARAM lp)
{
	char buf[46] = { 0 };
	// inet_ntopA가 호출되어 char 버퍼에 IP를 씁니다.
	inet_ntop(AF_INET, &wp, buf, sizeof(buf));

	CString ip(buf); // char* 버퍼로 CString(A)를 초기화합니다.
	CString port;
	port.Format(_T("%d"), lp);

	addListClient(ip, port);
	return 0;
}

LRESULT CNetworkDlg::OnDisconnectClient(WPARAM wp, LPARAM lp)
{
	char buf[46] = { 0 };
	inet_ntop(AF_INET, &wp, buf, sizeof(buf));

	CString ip(buf);
	CString port;
	port.Format(_T("%d"), lp);

	for (int row = m_list.GetItemCount() - 1; row >= 0; row--)
	{
		if (ip == m_list.GetItemText(row, 1) && port == m_list.GetItemText(row, 2))
		{
			m_list.DeleteItem(row);
			break;
		}
	}
	return 0;
}

LRESULT CNetworkDlg::OnRecevieNick(WPARAM wp, LPARAM lp)
{
	wstring wstr = Server::MultibyteToUnicode(CP_UTF8, (char*)wp, (int)lp);
	CString cstr(wstr.c_str()); // 유니코드 wstring을 MBCS CString으로 변환
	addListNick(cstr);
	return LRESULT();
}

LRESULT CNetworkDlg::OnRecevieText(WPARAM wp, LPARAM lp)
{
	wstring wstr = Server::MultibyteToUnicode(CP_UTF8, (char*)wp, (int)lp);
	CString cstr(wstr.c_str()); // 유니코드 wstring을 MBCS CString으로 변환
	addListText(cstr);
	return LRESULT();
}