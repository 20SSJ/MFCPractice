#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS WndClass; // 윈도우 클래스 구조체
	HWND hwnd;		   // 윈도우 핸들
	MSG msg;		   // 메시지 구조체

	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;	// 윈도우 프로시저
	WndClass.cbClsExtra = 0;		// 윈도우 클래스 데이터영역
	WndClass.cbWndExtra = 0;		// 윈도우 데이터 영역
	WndClass.hInstance = hInstance;	// 인스턴스 핸들
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 아이콘 핸들
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);   // 커서 핸들
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// 배경 브러시 핸들(밝은 회색배경)
	WndClass.lpszMenuName = NULL;			 // 메뉴 이름
	WndClass.lpszClassName = TEXT("Mclass"); // 윈도우 클래스 이름
	if (!RegisterClass(&WndClass)) return 1; // 윈도우 클래스 register

	hwnd = CreateWindow(			// 윈도우 생성 API 함수
		TEXT("MClass"),				// 등록된 윈도우 클래스 이름
		TEXT("마우스로 그림그리기"),  // 타이틀바에 출력될 문자
		WS_OVERLAPPEDWINDOW,	   // 윈도우 스타일
		CW_USEDEFAULT,			   // 윈도우좌측상단의 x좌표
		CW_USEDEFAULT,			   // 윈도우좌측상단의 y좌표
		CW_USEDEFAULT,			   // 윈도우의 너비
		CW_USEDEFAULT,			   // 윈도우의 높이
		NULL,					   // 부모 윈도우의 핸들
		NULL,					   // 메뉴 또는 자식 윈도우의 핸들
		hInstance,				   // 어플리케이션 인스턴스 핸들
		NULL					   // 윈도우 생성 데이터의 주소
	);
	ShowWindow(hwnd, nCmdShow);	   // 프레인 윈도우를 화면에 표시

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	static POINT MousePos; // 마우스 x,y 좌표를 저장할 변수

	static BOOL bDrawing = FALSE;

	switch (iMessage) {
		case WM_LBUTTONDOWN:
			MousePos.x = LOWORD(lParam);
			MousePos.y = HIWORD(lParam);
			bDrawing = TRUE;
			SetCapture(hWnd);
			return 0;
		case WM_MOUSEMOVE:
			if (bDrawing == TRUE) {
				hdc = GetDC(hWnd);
				MoveToEx(hdc, MousePos.x, MousePos.y, NULL);
				MousePos.x = LOWORD(lParam);
				MousePos.y = HIWORD(lParam);
				LineTo(hdc, MousePos.x, MousePos.y);
				ReleaseDC(hWnd, hdc);
			}
			return 0;
		case WM_LBUTTONUP:
			bDrawing = FALSE;
			ReleaseCapture();
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}