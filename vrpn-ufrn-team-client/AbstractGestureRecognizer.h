#pragma once
#include "VariabilitiesConfig.h"

#ifdef THERAPY_MODULE
#include "Storage.h"
#endif

class AbstractGestureRecognizer{

#ifdef THERAPY_MODULE
	protected:
	static Storage *storage;
	public:
	void setStorage(Storage *storage) {
		this->storage = storage;
	}
#endif

};