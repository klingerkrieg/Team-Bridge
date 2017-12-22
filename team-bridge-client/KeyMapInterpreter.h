#pragma once
/*#include "KeyMap.h"
#include "GestureRecognizer.h"
#include "AbstractAction.h"
#include "View.h"
#include "KeyMap.h"
#include "CheckerSubject.h"


#ifdef THERAPY_MODULE
#include "Storage.h"
#endif

class KeyMapInterpreter {

	private:
	static SkeletonPart skelPart;
	GestureRecognizer gr;
	AbstractAction *act;

	static int lastTimeTrack;
	bool viewOn = false;
	View *view;

#ifdef THERAPY_MODULE
	Storage *storage;
#endif

	public:


	KeyMapInterpreter(std::vector<KeyMap> &map, AbstractAction *act, std::map<std::string, CheckerSubject> &checkers) {
		init(act, checkers, map);
	}

	KeyMapInterpreter(std::vector<KeyMap> &map, AbstractAction *act, std::map<std::string, CheckerSubject> &checkers, View &view) {
		
		init(act, checkers, map);
		this->view = &view;

	}

	void init(AbstractAction *act, std::map<std::string, CheckerSubject> &checkers, std::vector<KeyMap> &map) {
		this->act = act;

		gr = GestureRecognizer();
		gr.GestureRecognizer::assignChecker(checkers, map);


	#ifdef THERAPY_MODULE
		gr.AbstractGestureRecognizer::setStorage(storage);
	#endif
	}

	void interpretKeyMap(KeyMap &keyMap);
	bool interpretOnLeave(bool active, KeyMap &keyMap);
};

*/