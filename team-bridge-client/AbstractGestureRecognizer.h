#pragma once
#include "VariabilitiesConfig.h"

#ifdef THERAPY_MODULE
#include "Storage.h"
#endif

class CheckerSubject;
class KeyMap;

class AbstractGestureRecognizer{

	protected:
	virtual bool assignChecker(CheckerSubject *checker, KeyMap *keyMap) = 0;

#ifdef THERAPY_MODULE
	protected:
	static Storage *storage;
	public:
	void setStorage(Storage *storage) {
		this->storage = storage;
	}
#endif

};