#include "View.h"
#include <windows.graphics.h>

// The main window class name.  
TCHAR View::szWindowClass[] = _T("win32app");
// The string that appears in the application's title bar.  
TCHAR View::szTitle[] = _T("Win32 Guided Tour Application");

std::string View::msgToShow;
bool View::isAlert;
int View::delay;

int View::lastTime = 0;

std::deque<HWND> View::hWndVect;
HWND View::hWnd;
HINSTANCE View::hInstance;
WNDCLASSEX View::wcex;
bool View::created = false;
int View::msgTop = 10;

//Font
int View::fontWidth = 32;
int View::fontHeight = 50;
int View::fontPadding = 5;


//GDI
Gdiplus::GdiplusStartupInput View::gdiplusStartupInput;
ULONG_PTR           View::gdiplusToken;




View::View() {
}

View::View(HINSTANCE hInstance) {

	if ( created ) {
		return;
	}
	
	this->hInstance = hInstance;
	//hInstance = GetModuleHandle(0);
	//HINSTANCE hPrevInstance = GetModuleHandle(0);
	LPSTR lpCmdLine = _T("");

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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
	//msgTop -= fontHeight + 10;
	//printf("Close window\n");
	if ( hWndVect.size() > 0 ) {
		PostMessage(hWndVect.at(0), WM_CLOSE, 0, 0);
		hWndVect.pop_front();
	}

	//PostMessage(hWnd, WM_CLOSE, 0, 0);
}


void View::call(bool alert, std::string text, int delay) {

	//Filtro para não permitir que a mesma mensagem seja acionada repetidas vezes em curto espaço de tempo
	int actTime = (int)time(0);
	if ( actTime - lastTime < (this->delay/1000) && text.compare(msgToShow) == 0) {
		return;
	}
	lastTime = actTime;
	

	isAlert = alert;
	this->delay = delay;
	msgToShow = text;
	replace_all(msgToShow, "\t", " ");//\t nao e exibido

	int nCmdShow = 1;
	
	//PostMessage(hWnd, WM_CLOSE, 0, 0);
	

	Sleep(100);//Espera fechar a janela
	std::thread([=] { show(hInstance, wcex); }).detach();
	
}




int WINAPI View::show(HINSTANCE hInstance, WNDCLASSEX wcex) {
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	//printf("Show window\n");

	DWORD Flags1 = WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT;
	DWORD Flags2 = WS_POPUP;

	HWND hWndC = CreateWindowEx(Flags1, szWindowClass, szTitle, Flags2, 10, msgTop, msgToShow.length()*fontWidth + (fontPadding * 2), 60, 0, 0, hInstance, 0);
	
	//hWnd = CreateWindowEx(Flags1, szWindowClass, szTitle, Flags2, 10, msgTop, msgToShow.length()*fontWidth + (fontPadding*2), 60, 0, 0, hInstance, 0);
	//msgTop += fontHeight+10;
	if ( !hWndC ) {
		printf("Call to CreateWindow failed!");
		return 1;
	}

	//Alpha
	int exStyle = GetWindowLong(hWndC, GWL_EXSTYLE);
	exStyle |= WS_EX_LAYERED;
	SetWindowLong(hWndC, GWL_EXSTYLE, exStyle);
	SetLayeredWindowAttributes(hWndC, NULL, 240, LWA_ALPHA);

	/*HRGN GGG = CreateRectRgn(0, 0, 1920, 1200);
	InvertRgn(GetDC(hWnd), GGG);
	SetWindowRgn(hWnd, GGG, false);

	COLORREF RRR = RGB(255, 0, 255);
	SetLayeredWindowAttributes(hWnd, RRR, (BYTE)0, LWA_COLORKEY);
	*/
	ShowWindow(hWndC, 1);
	UpdateWindow(hWndC);

	//DeleteObject(GGG);

	hWndVect.push_back(hWndC);

	std::thread([=] { sendClose();  }).detach();


	MSG msg;
	while ( GetMessage(&msg, NULL, 0, 0) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}


void View::OnPaint(HDC hdc, int width) {
	Gdiplus::Graphics g(hdc);
	Gdiplus::SolidBrush brush(Gdiplus::Color(127, 64, 64, 64));
	g.FillRectangle(&brush, 0, 0, width, 100);
}


LRESULT CALLBACK View::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {


	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;

	

	TCHAR *msg = new TCHAR[msgToShow.size() + 1];
	msg[msgToShow.size()] = 0;
	std::copy(msgToShow.begin(), msgToShow.end(), msg);


	HFONT hTmp;
	HFONT hFont = CreateFont(fontHeight, fontWidth, NULL, NULL, FW_MEDIUM, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Consolas");
	
	switch ( message ) {
		case WM_ERASEBKGND:

			//GetClientRect(hWnd, &rect);
			//FillRect((HDC)wParam, &rect, CreateSolidBrush(RGB(255, 0, 255)));

			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			hTmp = (HFONT)SelectObject(hdc, hFont);
			SetBkMode(hdc, TRANSPARENT);
			

			OnPaint(hdc, msgToShow.length() * fontWidth + (fontPadding*2));

			if ( isAlert ) {
				SetTextColor(hdc, RGB(174, 24, 24));
			} else {
				SetTextColor(hdc, RGB(173, 255, 47));
			}
		
			TextOut(hdc, fontPadding, fontPadding, msg, _tcslen(msg));

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

