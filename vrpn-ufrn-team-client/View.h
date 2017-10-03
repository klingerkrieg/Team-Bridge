#pragma once
#include <string>
#include <Python.h>
#include <thread>
#include <queue>

struct Msg {
	std::string msg;
	std::string funcName;
	PyObject *func;
};

class View {
private:
	
	
	static int lastMsg;
	void insertInQueue(std::string funcName, std::string text);
	
	static int msgDelay;


	
	
public:
	static bool queueStarted;
	static std::queue<Msg> msgs;
	static void call(std::string funcName, std::string text);
	View();
	void showAlert(std::string text);
	void showMsg(std::string text);
	void startQueue();

};


