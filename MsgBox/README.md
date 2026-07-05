# MsgBox — 버튼 이벤트와 메시지 박스 연습

MFC **대화 상자 기반(Dialog-based)** 애플리케이션으로, 가장 기초적인 **버튼 클릭 이벤트 처리**와 **MessageBox 띄우기**를 연습하는 프로젝트입니다.

## 프로그램 설명

버튼 두 개가 있는 작은 대화 상자입니다.

| 버튼 | 동작 |
|---|---|
| 인사 버튼 (`IDC_BUTTON_HELLO`) | "MFC WORLD에 오신것을 환영합니다." 메시지 박스를 띄움 |
| 종료 버튼 (`IDC_BUTTON_EXIT`) | `OnOK()`를 호출해서 대화 상자(프로그램)를 닫음 |

## 핵심 코드 위치

로직은 `MsgBoxDlg.cpp` 맨 아래 두 함수가 전부입니다.

```cpp
void CMsgBoxDlg::OnClickedButtonHello()
{
    MessageBox(_T("MFC WORLD에 오신것을 환영합니다."));
}

void CMsgBoxDlg::OnClickedButtonExit()
{
    OnOK();   // 대화 상자 닫기
}
```

버튼과 함수의 연결은 같은 파일의 **메시지 맵**에서 확인할 수 있습니다:

```cpp
BEGIN_MESSAGE_MAP(CMsgBoxDlg, CDialogEx)
    ...
    ON_BN_CLICKED(IDC_BUTTON_HELLO, &CMsgBoxDlg::OnClickedButtonHello)
    ON_BN_CLICKED(IDC_BUTTON_EXIT,  &CMsgBoxDlg::OnClickedButtonExit)
END_MESSAGE_MAP()
```

## 이 프로젝트에서 배우는 것

- MFC 대화 상자 기반 프로젝트의 기본 뼈대 (위저드가 생성하는 코드가 무엇인지)
- **메시지 맵**(`ON_BN_CLICKED`)으로 버튼 클릭 → 핸들러 함수 연결하는 구조
- `MessageBox()` 사용법
- `OnOK()`로 대화 상자를 정상 종료하는 방법

나머지 코드(`OnInitDialog`, `OnPaint`, `OnSysCommand` 등)는 위저드가 자동 생성한 기본 코드로, 아이콘 표시·정보(About) 대화 상자 처리용입니다. 직접 작성한 부분이 아니므로 그대로 두면 됩니다.

## 빌드/실행 방법

1. Visual Studio에서 `MsgBox.sln` 열기 (MFC 구성 요소 설치 필요)
2. F5(디버그 실행) 또는 Ctrl+F5

## 파일 구성

```
MsgBox/
├── MsgBox.sln / .vcxproj        # 솔루션·프로젝트 파일
├── MsgBox.cpp / .h              # 앱 클래스 (CWinApp) — 프로그램 시작점
├── MsgBoxDlg.cpp / .h           # ★ 메인 대화 상자 — 버튼 핸들러가 여기 있음
├── MsgBox.rc / resource.h       # 대화 상자 레이아웃, 버튼 ID 정의
├── pch.h / framework.h / targetver.h  # 위저드가 만든 공통 헤더
└── res/                         # 아이콘 등 리소스
```
