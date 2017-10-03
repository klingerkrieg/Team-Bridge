#include "View.h"


std::queue<Msg> View::msgs;
int View::lastMsg = 0;
int View::msgDelay = 3000;
bool View::queueStarted = false;


View::View() {
	
}

void View::showAlert(std::string text) {
	insertInQueue("alert", text);
}

void View::showMsg(std::string text) {
	insertInQueue("msg", text);
}

void View::startQueue() {
	//std::this_thread::sleep_for(std::chrono::seconds(sleep));
	printf("queueStarted\n");

	while ( msgs.size() > 0 ) {
		
		Msg it = msgs.front();
		msgs.pop();
		call(it.funcName, it.msg);
		//call(it.func, it.msg);
		//std::this_thread::sleep_for(std::chrono::milliseconds(msgDelay + 3000));

		
	}
	queueStarted = false;
}



void next() {
	if ( View::msgs.size() > 0 ) {

		Msg it = View::msgs.front();
		View::msgs.pop();
		View::call(it.funcName, it.msg);

	} else {
		View::queueStarted = false;
	}
}

void View::insertInQueue(std::string funcName,std::string text) {
	
	Msg msg;
	msg.funcName = funcName;
	msg.msg = text;
	msgs.push(msg);
	int actual = (int)time(0);
	
	if ( queueStarted == false ) {
		queueStarted = true;
		//std::thread([=] { startQueue(); }).detach();
		next();
	}
	
	printf("wait %s\n", text.c_str());
	
}






void call1(std::string funcName, std::string text) {
	printf("call %s with %s\n", funcName.c_str(), text.c_str());

	PyObject *pName, *pFunc, *pModule;
	char moduleName[] = "view";

	Py_Initialize();
	pName = PyString_FromString(moduleName);
	//Error checking of pName left out 
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if ( pModule != NULL ) {
		pFunc = PyObject_GetAttrString(pModule, funcName.c_str());
	} else {
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", moduleName);
	}

	PyObject *pArgs, *pValue;

	if ( pFunc && PyCallable_Check(pFunc) ) {


		pArgs = PyTuple_New(2);
		//texto
		pValue = PyString_FromString(text.c_str());

		if ( !pValue ) {
			Py_DECREF(pArgs);
			Py_DECREF(pModule);
			fprintf(stderr, "Cannot convert argument\n");
			return;
		}

		PyTuple_SetItem(pArgs, 0, pValue);
		//tempo
		pValue = PyInt_FromLong(3000);

		if ( !pValue ) {
			Py_DECREF(pArgs);
			Py_DECREF(pModule);
			fprintf(stderr, "Cannot convert argument\n");
			return;
		}
		PyTuple_SetItem(pArgs, 1, pValue);


		printf("call py\n");
		PyObject_CallObject(pFunc, pArgs );
		
		//std::thread(PyObject_CallObject, pFunc, pArgs).detach();
		//std::thread([=] {PyObject_CallObject(pFunc, pArgs); }).detach();


	} else if ( PyErr_Occurred() ) {
		PyErr_Print();
		fprintf(stderr, "Cannot find function \"%s\"\n", (char *)pFunc);
	}

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);
	Py_Finalize();

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	next();
}


void View::call(std::string funcName, std::string text) {

	std::thread(call1, funcName, text).detach();

	/**/
}
