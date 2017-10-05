#pragma once
#include <string>
#include <Python.h>


class View {
private:
	int msgDelay = 1500;
	PyObject *pName, *pModule, *pFuncMsg, *pFuncAlert;

public:
	View();
	View::~View();
	void showAlert(std::string text);
	void showMsg(std::string text);
	void call(PyObject *pFunc, std::string text);

	void showAlert(std::string text, int delay);
	void showMsg(std::string text, int delay);
	void call(PyObject *pFunc, std::string text, int delay);

};


