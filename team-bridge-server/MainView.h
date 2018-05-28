#pragma once

#include <windows.h>
#include "resource.h"
#include <tchar.h>  
#include <string>


const int MAIN_VIEW_KINECT_V1 = 1;
const int MAIN_VIEW_KINECT_V2 = 2;


class MainView {
	
	public:
	/// <summary>
	/// Constructor
	/// </summary>
	MainView();

	/// <summary>
	/// Destructor
	/// </summary>
	~MainView();

	/// <summary>
	/// Register class and create window
	/// </summary>
	/// <returns>S_OK for success, or failure code</returns>
	HRESULT                             InitWindow(HINSTANCE hInstance, int nCmdShow);


	/// <summary>
	/// Handles window messages, used to process input
	/// </summary>
	/// <param name="hWnd">window message is for</param>
	/// <param name="uMsg">message</param>
	/// <param name="wParam">message data</param>
	/// <param name="lParam">additional message data</param>
	/// <returns>result of message processing</returns>
	LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	static void write(std::string text);
	static void writeln(std::string text);
	static std::string getOutput();
	static void startDeviceView(int device);
	bool MainView::getDeviceView(int device);

	private:

	static bool							kinectV1View;
	static bool							kinectV2View;

	static std::string					output;

	HINSTANCE                           m_hInst;
	HWND                                m_hWnd;
	
	float                               m_xyScale;

	// Initial window resolution
	int                                 m_windowResX;
	int                                 m_windowResY;

};