#pragma once
#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include <chrono>
#include <thread>

class View {
private:
	int msgDelay = 3000;
	

	// The main window class name.  
	static TCHAR szWindowClass[];

	// The string that appears in the application's title bar.  
	static TCHAR szTitle[];

	HINSTANCE hInst;
	HWND hWnd;

	static HINSTANCE hInstance;
	static WNDCLASSEX wcex;
	static bool created;

	static int delay;
	static bool isAlert;
	static std::string msgToShow;
	static int msgTop;

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void sendClose();

public:

	View();
	View(HINSTANCE hInstance);
	View::~View();
	void showAlert(std::string text);
	void showMsg(std::string text);

	int WINAPI View::show(HINSTANCE hInstance, WNDCLASSEX wcex);

	void showAlert(std::string text, int delay);
	void showMsg(std::string text, int delay);

	void call(bool alert, std::string text, int delay);

};


class WithoutHInstance : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Voce precisa enviar o HINSTANCE na primeira vez que for criar o View";
	}

	void noZero() {

	}

};