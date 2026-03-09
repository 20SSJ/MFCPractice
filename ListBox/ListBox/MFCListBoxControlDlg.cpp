
// MFCListBoxControlDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCListBoxControl.h"
#include "MFCListBoxControlDlg.h"
#include "afxdialogex.h"

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


// CMFCListBoxControlDlg 대화 상자



CMFCListBoxControlDlg::CMFCListBoxControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCLISTBOXCONTROL_DIALOG, pParent)
	, m_strEtc(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCListBoxControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MEAL, m_listMeal);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
	DDX_Control(pDX, IDC_LIST_DESSERT, m_listDessert);
	DDX_Text(pDX, IDC_EDIT_ETC, m_strEtc);
}

BEGIN_MESSAGE_MAP(CMFCListBoxControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_MEAL, &CMFCListBoxControlDlg::OnClickedButtonMeal)
	ON_BN_CLICKED(IDC_BUTTON_DESSERT, &CMFCListBoxControlDlg::OnClickedButtonDessert)
	ON_BN_CLICKED(IDC_BUTTON_ETC, &CMFCListBoxControlDlg::OnClickedButtonEtc)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CMFCListBoxControlDlg::OnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMFCListBoxControlDlg::OnClickedButtonReset)
END_MESSAGE_MAP()


// CMFCListBoxControlDlg 메시지 처리기

BOOL CMFCListBoxControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_listMeal.AddString(_T("산채비빔밥"));
	m_listMeal.AddString(_T("안동찜닭"));
	m_listMeal.AddString(_T("두부조림"));
	m_listMeal.AddString(_T("꿔바로우"));
	m_listMeal.AddString(_T("고기산적"));
	m_listMeal.AddString(_T("들깨백불고기"));
	m_listMeal.AddString(_T("잡곡밥"));
	m_listMeal.AddString(_T("백미밥"));

	m_listDessert.AddString(_T("커피"));
	m_listDessert.AddString(_T("식혜"));
	m_listDessert.AddString(_T("경단"));
	m_listDessert.AddString(_T("녹차"));

	m_listDessert.AddString(_T("사과주스"));
	m_listDessert.AddString(_T("오렌지주스"));

	m_listDessert.AddString(_T("길거리토스트"));
	m_listDessert.AddString(_T("쿠키"));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCListBoxControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCListBoxControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCListBoxControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCListBoxControlDlg::OnClickedButtonMeal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int Index = m_listMeal.GetCurSel();
	if (Index != LB_ERR) {
		CString str;
		m_listMeal.GetText(Index, str);
		m_listMeal.SetCurSel(Index);
		m_listResult.AddString(str);
	}
}

void CMFCListBoxControlDlg::OnClickedButtonDessert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	int count = m_listDessert.GetCount();

	for (int i = count - 1; i >= 0; i--) {
		if (m_listDessert.GetSel(i)) {
			m_listDessert.GetText(i, str);
			m_listResult.AddString(str);
		}
	}
}

void CMFCListBoxControlDlg::OnClickedButtonEtc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ETC);
	UpdateData(TRUE);

	if (m_strEtc.IsEmpty()) {
		MessageBox(_T("기타 메뉴를 입력하세요"), _T("Error"), MB_ICONSTOP | MB_OK);
		pEdit->SetFocus();
		return;
	}
	str = m_strEtc;
	m_listResult.AddString(str);
	UpdateData(FALSE);
}

void CMFCListBoxControlDlg::OnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex;
	nIndex = m_listResult.GetCurSel();
	if (nIndex != LB_ERR) {
		m_listResult.DeleteString(nIndex);
	}
	else {
		AfxMessageBox(_T("삭제할 메뉴를 선택하세요"));
	}
}

void CMFCListBoxControlDlg::OnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ETC);
	UpdateData(TRUE);

	m_strEtc = _T("");
	pEdit->SetFocus();
	UpdateData(FALSE);
}
