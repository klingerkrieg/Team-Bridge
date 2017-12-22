/*#include "KeyMapInterpreter.h"

SkeletonPart KeyMapInterpreter::skelPart;


void KeyMapInterpreter::interpretKeyMap(KeyMap &keyMap) {

	if ( keyMap.getToKey() == KINECT_SET_CENTER_POS ) {
		gr.setCenterPos(&skelPart, &keyMap);
		printf("Center pos defined.");
		if ( viewOn )
			view->showMsg("Posição definida.");
	} else
	if ( keyMap.getToKey() == ALERT ) {
		if ( viewOn )
			view->showAlert(keyMap.getMsg());
	} else
	if ( keyMap.getToKey() == MESSAGE ) {
		if ( viewOn )
			view->showMsg(keyMap.getMsg());
	} else {
		//Executa a ação, para essa aplicação são inputs de mouse e teclado
		act->run(keyMap);
	}
}


bool KeyMapInterpreter::interpretOnLeave(bool active, KeyMap &keyMap) {

	//Se esta esperando para soltar a tecla e a tecla foi solta
	if ( keyMap.getActive() && !active ) {

		if ( keyMap.getHasOnLeave() ) {
			interpretKeyMap((*keyMap.getOnLeave()));
		}
		keyMap.setActive(false);
		return true;
	} else
		if ( active ) {
			//Se foi ativada
			if ( keyMap.getHasOnLeave() ) {
				//Verifica se tem evento de saida
				if ( !keyMap.getActive() ) {
					//Se tiver so aciona o comando novamente se ele nao estiver esperando evento de saida
					interpretKeyMap(keyMap);
				}
				keyMap.setActive(true);
			} else {

				if ( keyMap.getActive() == false ) {
					//Se nao tiver evento de saida aciona normalmente
					interpretKeyMap(keyMap);
				}

				//se não tem hasOnLeave
				if ( keyMap.getBtnUp() == false ) {
					keyMap.setActive(true);
				}

			}
			return true;
		}
	return false;
}*/