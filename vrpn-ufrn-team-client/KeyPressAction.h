#pragma once
#include "AbstractAction.h"


class KeyPressAction : public AbstractAction{

	private:
	static bool mouseLeftPressed;
	static bool mouseRightPressed;
	static bool mouseMiddlePressed;

	public:
	void run(KeyMap key);

};
