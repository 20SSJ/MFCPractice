
// MFCListBoxControlDlg.h: 헤더 파일
//

#pragma once


// CMFCListBoxControlDlg 대화 상자
class CMFCListBoxControlDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCListBoxControlDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCLISTBOXCONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listMeal;
	CListBox m_listResult;
	CListBox m_listDessert;
	afx_msg void OnClickedButtonMeal();
	afx_msg void OnClickedButtonDessert();
	CString m_strEtc;
	afx_msg void OnClickedButtonEtc();
	afx_msg void OnClickedButtonDel();
	afx_msg void OnClickedButtonReset();
};
