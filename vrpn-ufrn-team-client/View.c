#include "View.h"



View::View() {

	char moduleName[] = "view";

	Py_Initialize();
	pName = PyString_FromString(moduleName);
	//Error checking of pName left out 
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if ( pModule != NULL ) {
		pFuncMsg = PyObject_GetAttrString(pModule, "msg");
		pFuncAlert = PyObject_GetAttrString(pModule, "alert");
	} else {
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", moduleName);
	}
	
}

View::~View() {
	Py_XDECREF(pFuncMsg);
	Py_XDECREF(pFuncAlert);
	Py_DECREF(pModule);
	Py_Finalize();
}

void View::showAlert(std::string text) {
	call(pFuncAlert, text);
}

void View::showMsg(std::string text) {
	call(pFuncMsg, text);
}



void View::call(PyObject *pFunc, std::string text) {
	printf("call with %s\n",text.c_str());

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
		pValue = PyInt_FromLong(msgDelay);

		if ( !pValue ) {
			Py_DECREF(pArgs);
			Py_DECREF(pModule);
			fprintf(stderr, "Cannot convert argument\n");
			return;
		}
		PyTuple_SetItem(pArgs, 1, pValue);


		printf("call py\n");
		PyObject_CallObject(pFunc, pArgs );

	} else if ( PyErr_Occurred() ) {
		PyErr_Print();
		fprintf(stderr, "Cannot find function \"%s\"\n", (char *)pFunc);
	}

	

}
