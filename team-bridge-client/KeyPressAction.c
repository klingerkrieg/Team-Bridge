#include "KeyPressAction.h"

bool KeyPressAction::mouseLeftPressed = false;
bool KeyPressAction::mouseRightPressed = false;
bool KeyPressAction::mouseMiddlePressed = false;

void KeyPressAction::run(KeyMap key) {

	bool print = true;
	int actualTime = (int)time(0);

	if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON
		|| key.getToKey() == VK_MOUSEMOVE
		|| key.getToKey() == VK_RBUTTON_DOWN
		|| key.getToKey() == VK_LBUTTON_DOWN
		|| key.getToKey() == VK_RBUTTON_UP
		|| key.getToKey() == VK_LBUTTON_UP
		|| key.getToKey() == VK_MBUTTON_UP
		|| key.getToKey() == VK_MBUTTON_UP ) {


		INPUT input, inputUp;
		input.type = INPUT_MOUSE;//Ele automaticamente soma com a posicao atual do mouse
		inputUp.type = INPUT_MOUSE;

		input.mi.mouseData = 0;
		input.mi.time = 0;
		inputUp.mi.mouseData = 0;
		inputUp.mi.time = 0;
		bool mouseMove = false;

		if ( key.getToKey() == VK_MOUSEMOVE ) {
			input.mi.dwFlags = MOUSEEVENTF_MOVE;
			input.mi.dx = key.getMouseX();
			input.mi.dy = key.getMouseY();
			//std::cout << "move to x:" << input.mi.dx << "y:" << input.mi.dy << "\n";
			mouseMove = true;
		} else
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_RBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			//Se o botao ja esta pressionado nao pressiona de novo
			if ( mouseRightPressed ) {
				return;
			}
			mouseRightPressed = true;
		} else
		if ( key.getToKey() == VK_LBUTTON || key.getToKey() == VK_LBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			//Se o botao ja esta pressionado nao pressiona de novo
			if ( mouseLeftPressed ) {
				return;
			}
			mouseLeftPressed = true;
		} else
		if ( key.getToKey() == VK_MBUTTON || key.getToKey() == VK_MBUTTON_DOWN ) {
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			inputUp.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			if ( mouseMiddlePressed ) {
				return;
			}
			mouseMiddlePressed = true;
		} else
		if ( key.getToKey() == VK_RBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			//Se o botao nao esta pressionado, nao tem nada pra despressionar
			if ( mouseRightPressed == false ) {
				return;
			}
			mouseRightPressed = false;
		} else
		if ( key.getToKey() == VK_LBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			if ( mouseLeftPressed == false ) {
				return;
			}
			mouseLeftPressed = false;
		} else
		if ( key.getToKey() == VK_MBUTTON_UP ) {
			input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			if ( mouseMiddlePressed == false ) {
				return;
			}
			mouseMiddlePressed = false;
		}

		if ( print && (mouseMiddlePressed || mouseLeftPressed || mouseRightPressed || mouseMove) )
			printf("Press: %s on windows.\n", key.getToKeyRepr().c_str());
		else if ( print && (!mouseMiddlePressed && !mouseLeftPressed && !mouseRightPressed && !mouseMove) )
			printf("Unpress: %s on windows.\n", key.getToKeyRepr().c_str());

		SendInput(1, &input, sizeof(INPUT));
		ZeroMemory(&input, sizeof(INPUT));
		//So vai pressionar o soltar se for o evento normal
		if ( key.getToKey() == VK_RBUTTON || key.getToKey() == VK_LBUTTON || key.getToKey() == VK_MBUTTON ) {
			SendInput(1, &inputUp, sizeof(INPUT));
			ZeroMemory(&inputUp, sizeof(INPUT));

			//Caso tenha sido os dois eventos em um só
			if ( key.getToKey() == VK_LBUTTON ) {
				mouseLeftPressed = false;
			} else
			if ( key.getToKey() == VK_RBUTTON ) {
				mouseRightPressed = false;
			} else
			if ( key.getToKey() == VK_MBUTTON ) {
				mouseMiddlePressed = false;
			}
		}

	} else {

		INPUT input;
		WORD vkey = key.getToKey();
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		input.ki.wVk = vkey;

		if ( key.isKeyUpEvent() == false && key.getToKeyDown() || key.getToKeyWhile() ) {
			input.ki.dwFlags = 0;// there is no KEYEVENTF_KEYDOWN
			SendInput(1, &input, sizeof(INPUT));

			//Um delay para o game realizar a acao
			//Sleep(10);
			if ( print )
				printf("Press: %s on Windows [%d].\n", key.getToKeyRepr().c_str(), actualTime);
		}
		if ( key.isKeyUpEvent() || key.getToKeyWhile() ) {
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input, sizeof(INPUT));

			if ( print )
				printf("Unpress: %s on Windows [%d].\n", key.getToKeyRepr().c_str(), actualTime);
		}

	}

}