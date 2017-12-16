#pragma once
#include "KeyMap.h"

class AbstractAction {
	public:
	virtual void run(KeyMap key) = 0;
	virtual ~AbstractAction(){}
};