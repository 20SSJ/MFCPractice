# ListBox — 리스트 박스 컨트롤 연습 (식단 고르기)

MFC **대화 상자 기반(Dialog-based)** 애플리케이션으로, **CListBox 컨트롤** 사용법을 연습하는 프로젝트입니다.
구내식당 메뉴판처럼 식사/후식 목록에서 항목을 골라 "선택 결과" 리스트에 담는 프로그램입니다.

## 프로그램 설명

- **식사 리스트** (단일 선택): 산채비빔밥, 안동찜닭, 두부조림 등 8가지가 미리 들어 있음
- **후식 리스트** (다중 선택): 커피, 식혜, 경단 등 8가지가 미리 들어 있음
- **기타 입력란**: 목록에 없는 메뉴를 직접 입력
- **결과 리스트**: 위에서 고른 항목들이 모이는 곳

| 버튼 | 동작 |
|---|---|
| 식사 선택 | 식사 리스트에서 선택한 **한 개** 항목을 결과 리스트에 추가 |
| 후식 선택 | 후식 리스트에서 선택한 **여러 개** 항목을 한 번에 결과 리스트에 추가 |
| 기타 추가 | 에디트 박스에 입력한 텍스트를 결과 리스트에 추가 (비어 있으면 에러 메시지 박스) |
| 삭제 | 결과 리스트에서 선택한 항목 삭제 (선택 안 했으면 안내 메시지) |
| 리셋 | 기타 입력란을 비우고 포커스를 되돌림 |

## 핵심 코드 위치

모든 로직은 `ListBox/MFCListBoxControlDlg.cpp`에 있습니다.

- **초기 데이터 채우기**: `OnInitDialog()` — `AddString()`으로 식사/후식 목록을 채움
- **단일 선택 읽기**: `OnClickedButtonMeal()` — `GetCurSel()` + `GetText()`
- **다중 선택 읽기**: `OnClickedButtonDessert()` — `GetCount()`로 전체를 돌면서 `GetSel(i)`로 선택 여부 확인 (뒤에서부터 순회)
- **에디트 박스 ↔ 변수 동기화**: `OnClickedButtonEtc()` — `UpdateData(TRUE/FALSE)`와 DDX 변수 `m_strEtc` 사용
- **항목 삭제**: `OnClickedButtonDel()` — `GetCurSel()` + `DeleteString()`

컨트롤과 변수 연결(DDX)은 `DoDataExchange()`에서 합니다:
`m_listMeal`(식사), `m_listDessert`(후식), `m_listResult`(결과), `m_strEtc`(기타 입력 문자열).

## 이 프로젝트에서 배우는 것

- `CListBox`의 기본 메서드: `AddString`, `GetCurSel`, `GetText`, `GetSel`, `GetCount`, `DeleteString`
- 단일 선택 리스트 박스와 **다중 선택(Multiple Selection)** 리스트 박스의 차이
  - 단일 선택: `GetCurSel()` 하나로 끝
  - 다중 선택: `GetCurSel()`을 못 쓰고, 전체 항목을 돌며 `GetSel()`로 확인해야 함
- DDX(`DDX_Control`, `DDX_Text`)와 `UpdateData()`를 이용한 컨트롤-변수 데이터 교환
- `MessageBox` / `AfxMessageBox`로 입력 검증 결과 알려주기

## 빌드/실행 방법

1. Visual Studio에서 `ListBox/MFCListBoxControl.sln` 열기 (MFC 구성 요소 설치 필요)
2. F5(디버그 실행) 또는 Ctrl+F5

## 파일 구성

```
ListBox/ListBox/
├── MFCListBoxControl.sln / .vcxproj   # 솔루션·프로젝트 파일
├── MFCListBoxControl.cpp / .h         # 앱 클래스 (CWinApp) — 프로그램 시작점
├── MFCListBoxControlDlg.cpp / .h      # ★ 메인 대화 상자 — 실제 연습 코드는 전부 여기
├── MFCListBoxControl.rc / resource.h  # 대화 상자 레이아웃, 컨트롤 ID 정의
├── pch.h / framework.h / targetver.h  # 위저드가 만든 공통 헤더 (수정할 일 거의 없음)
└── res/                               # 아이콘 등 리소스
```
