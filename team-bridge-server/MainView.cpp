
#include "MainView.h"
#include "KinectView.h"
#include "TeamBridgeServer.h"
#include <thread>

MainView g_Application;  // Application class


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



/// <summary>
/// Constructor
/// </summary>
MainView::MainView() {
	// get resolution as DWORDS, but store as LONGs to avoid casts later
	DWORD width = 0;
	DWORD height = 0;

	m_hInst = NULL;
	m_hWnd = NULL;


	m_xyScale = 0.0f;

	// Initial window resolution
	m_windowResX = 600;
	m_windowResY = 480;
}

/// <summary>
/// Destructor
/// </summary>
MainView::~MainView() {
	

}


/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if ( FAILED(g_Application.InitWindow(hInstance, nCmdShow)) ) {
		return 0;
	}

	TeamBridgeServer tbServer = TeamBridgeServer();
	tbServer.start();
	std::thread servThread(&TeamBridgeServer::mainloop, &tbServer);

	KinectView kView = KinectView(hInstance, nCmdShow);
	kView.startView();


	// Main message loop
	MSG msg = { 0 };
	while ( WM_QUIT != msg.message ) {
		if ( PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) ) {
			
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		} else {
			
			if ( KinectView::getSensorStarted() ) {
				kView.Render();
			}
		}

	}

	tbServer.stop();
	servThread.join();
	//Encerra a thread
	Sleep(3000);

	return (int)msg.wParam;
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;

	g_Application.HandleMessages(hWnd, message, wParam, lParam);

	switch ( message ) {
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}



/// <summary>
/// Register class and create window
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT MainView::InitWindow(HINSTANCE hInstance, int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, (LPCTSTR)IDI_APP);
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TeamBridgeServerClass";
	wcex.hIconSm = LoadIconW(wcex.hInstance, (LPCTSTR)IDI_APP);

	if ( !RegisterClassEx(&wcex) ) {
		return E_FAIL;
	}

	// Create window
	m_hInst = hInstance;
	RECT rc = { 0, 0, m_windowResX, m_windowResY };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(L"TeamBridgeServerClass", L"TeamBridgeServer", WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
						  NULL);
	if ( NULL == m_hWnd ) {
		return E_FAIL;
	}

	ShowWindow(m_hWnd, nCmdShow);

	return S_OK;
}



/// <summary>
/// Handles window messages, used to process input
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT MainView::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch ( uMsg ) {
		case WM_SIZE:
			if ( SIZE_MINIMIZED == wParam ) {
				//m_bPaused = true;
			}
			break;

		case WM_ACTIVATEAPP:
			if ( wParam == TRUE ) {
				//m_bPaused = false;
			}
			break;

		case WM_KEYDOWN:
			{
				int nKey = static_cast<int>(wParam);

				if ( nKey == 'N' ) {
					//ToggleNearMode();
				}
				break;
			}
	}

	return 0;
}