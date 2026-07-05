# Menu_Toolbar — 메뉴·툴바 커스터마이징 연습 (SDI)

MFC **SDI(단일 문서 인터페이스)** 애플리케이션으로, 위저드의 "Visual Studio 스타일" 템플릿을 기반으로 **메인 메뉴에 항목을 직접 추가하고 명령 핸들러를 연결하는 것**을 연습하는 프로젝트입니다.

## 프로그램 설명

Visual Studio처럼 생긴 프레임 창이 뜹니다. 도킹 창(클래스 뷰, 파일 뷰, 출력 창, 속성 창)과 툴바는 위저드가 자동으로 만들어 준 것이고, **직접 추가한 부분은 메인 메뉴**입니다:

| 메뉴 | 하위 항목 | 상태 |
|---|---|---|
| 문자열 입력 | 입력 (`ID_DATA_INPUT`) | 핸들러 연결됨 — `CTextDlg` 객체를 만드는 것까지 작성 (대화 상자 띄우기는 미완성) |
| 글자색 | 빨간색글자, 파란색글자, 노란색글자 | 메뉴만 추가, 핸들러 없음 → 실행 시 비활성(회색) 표시 |
| 도형 | 원, 사각형, 선, 십자무늬 패턴, 사과무늬 패턴 | 메뉴만 추가, 핸들러 없음 → 실행 시 비활성(회색) 표시 |

> MFC는 `ON_COMMAND` 핸들러가 없는 메뉴 항목을 자동으로 회색 처리합니다. 글자색/도형 메뉴가 눌리지 않는 것은 버그가 아니라 아직 핸들러를 안 만든 것입니다.

## 핵심 코드 위치

- **메뉴 정의**: `MenuToolbar.rc`의 `IDR_MAINFRAME MENU` 블록 — 문자열 입력/글자색/도형 메뉴가 여기 정의되어 있음 (리소스 뷰에서 편집)
- **메뉴 명령 핸들러**: `Menu_ToolbarView.cpp`
  ```cpp
  ON_COMMAND(ID_DATA_INPUT, &CMenuToolbarView::OnDataInput)   // 메시지 맵

  void CMenuToolbarView::OnDataInput()
  {
      CTextDlg m_TextDlg;   // 입력 대화 상자 (아직 DoModal() 호출 전 단계)
  }
  ```
- **입력용 대화 상자 클래스**: `TextDlg.h` / `TextDlg.cpp` — `m_strTextInput` 멤버만 있는 뼈대 상태
- **우클릭 컨텍스트 메뉴**: `Menu_ToolbarView.cpp`의 `OnContextMenu()` — 뷰에서 우클릭하면 `IDR_POPUP_EDIT` 팝업 메뉴가 뜸

## 이 프로젝트에서 배우는 것

- SDI 애플리케이션의 구조: **App(`Menu_Toolbar.cpp`) → MainFrame(`MainFrm.cpp`) → Document(`Menu_ToolbarDoc.cpp`) → View(`Menu_ToolbarView.cpp`)** 로 역할이 나뉘는 문서/뷰 구조
- 리소스 편집기로 **메인 메뉴에 새 팝업/항목 추가**하고 ID 부여하기
- `ON_COMMAND` 메시지 맵으로 메뉴 항목 → View 클래스의 핸들러 함수 연결하기
- 핸들러가 없는 메뉴가 자동으로 비활성화되는 MFC의 command routing 동작
- 컨텍스트 메뉴(`ShowPopupMenu`) 동작 방식

## 빌드/실행 방법

1. Visual Studio에서 `Menu_Toolbar.sln` 열기 (MFC 구성 요소 설치 필요)
2. F5(디버그 실행) 또는 Ctrl+F5

## 파일 구성

```
Menu_Toolbar/
├── Menu_Toolbar.sln / .vcxproj      # 솔루션·프로젝트 파일
├── Menu_Toolbar.cpp / .h            # 앱 클래스 (CWinApp) — 프로그램 시작점
├── MainFrm.cpp / .h                 # 메인 프레임 창 — 메뉴바·툴바·도킹 창 생성
├── Menu_ToolbarDoc.cpp / .h         # 문서 클래스 (데이터 담당)
├── Menu_ToolbarView.cpp / .h        # ★ 뷰 클래스 — 추가한 메뉴 핸들러가 여기 있음
├── TextDlg.cpp / .h                 # ★ 문자열 입력용 대화 상자 클래스 (작성 중)
├── MenuToolbar.rc / resource.h      # ★ 메뉴·툴바·대화 상자 리소스 정의
├── ClassView / FileView / OutputWnd / PropertiesWnd / ViewTree (.cpp/.h)
│                                    # 위저드가 만든 도킹 창들 (수정 안 함)
├── pch.h / framework.h / targetver.h  # 위저드가 만든 공통 헤더
└── res/                             # 아이콘, 툴바 비트맵 등 리소스
```
