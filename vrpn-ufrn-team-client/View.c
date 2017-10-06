#include "View.h"
#include <windows.graphics.h>

// The main window class name.  
TCHAR View::szWindowClass[] = _T("win32app");
// The string that appears in the application's title bar.  
TCHAR View::szTitle[] = _T("Win32 Guided Tour Application");

std::string View::msgToShow;
bool View::isAlert;
int View::delay;

HINSTANCE View::hInstance;
WNDCLASSEX View::wcex;
bool View::created = false;


View::View() {
}

View::View(HINSTANCE hInstance) {

	if ( created ) {
		return;
	}
	printf("create");
	this->hInstance = hInstance;
	//hInstance = GetModuleHandle(0);
	//HINSTANCE hPrevInstance = GetModuleHandle(0);
	LPSTR lpCmdLine = _T("");

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &View::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if ( !RegisterClassEx(&wcex) ) {
		printf("Call to RegisterClassEx failed!\n");
		return;
	}
	created = true;
}

View::~View() {
	
}

void View::showAlert(std::string text) {
	call(true, text, msgDelay);
}

void View::showMsg(std::string text) {
	call(false, text, msgDelay);
}

void View::showAlert(std::string text, int delay) {
	call(true, text,delay);
}

void View::showMsg(std::string text, int delay) {
	call(false, text,delay);
}


void View::sendClose() {
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	PostMessage(hWnd, WM_CLOSE, 0, 0);
}


void View::call(bool alert, std::string text, int delay) {

	isAlert = alert;
	this->delay = delay;
	msgToShow = text;

	int nCmdShow = 1;
	

	std::thread([=] { show(hInstance, wcex); }).detach();
	
}




int WINAPI View::show(HINSTANCE hInstance, WNDCLASSEX wcex) {



	hInst = hInstance; 

	DWORD Flags1 = WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT;
	DWORD Flags2 = WS_POPUP;

	hWnd = CreateWindowEx(Flags1, szWindowClass, szTitle, Flags2, 30, 30, 800, 100, 0, 0, hInstance, 0);

	if ( !hWnd ) {
		printf("Call to CreateWindow failed!");
		return 1;
	}


	HRGN GGG = CreateRectRgn(0, 0, 1920, 1200);
	InvertRgn(GetDC(hWnd), GGG);
	SetWindowRgn(hWnd, GGG, false);

	COLORREF RRR = RGB(255, 0, 255);
	SetLayeredWindowAttributes(hWnd, RRR, (BYTE)0, LWA_COLORKEY);

	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);

	DeleteObject(GGG);

	std::thread([=] { sendClose();  }).detach();


	MSG msg;
	while ( GetMessage(&msg, NULL, 0, 0) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK View::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {


	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;

	
	TCHAR *msg = new TCHAR[msgToShow.size() + 1];
	msg[msgToShow.size()] = 0;
	std::copy(msgToShow.begin(), msgToShow.end(), msg);


	HFONT hTmp;
	HFONT hFont = CreateFont(60, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "SYSTEM_FIXED_FONT");

	switch ( message ) {
		case WM_ERASEBKGND:

			GetClientRect(hWnd, &rect);
			FillRect((HDC)wParam, &rect, CreateSolidBrush(RGB(255, 0, 255)));

			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			hTmp = (HFONT)SelectObject(hdc, hFont);
			SetBkMode(hdc, TRANSPARENT);
			

			if ( isAlert ) {
				SetTextColor(hdc, RGB(174, 24, 24));
			} else {
				SetTextColor(hdc, RGB(173, 255, 47));
			}
		
			TextOut(hdc, 0, 0, msg, _tcslen(msg));

			/*FontFamily  fontFamily(L"Times New Roman");
			Font        font(&fontFamily, 32, FontStyleRegular, UnitPixel);
			SolidBrush  solidBrush(Color(255, 0, 0, 255));
			WCHAR       string1[] = L"SingleBitPerPixel";
			WCHAR       string2[] = L"AntiAlias";

			graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);
			graphics.DrawString(string1, -1, &font, PointF(10.0f, 10.0f), &solidBrush);

			graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
			graphics.DrawString(string2, -1, &font, PointF(10.0f, 60.0f), &solidBrush);*/
			
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	
	return 0;
}

