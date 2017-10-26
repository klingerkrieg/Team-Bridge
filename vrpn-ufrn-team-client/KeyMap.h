#pragma once
#include <map>
#include <string>
#include <vector>
#include <Windows.h>
#include <iostream>

#include "util.h"


static const int SSIZE = 512;

const int VK_MOUSEMOVE = 4000;
const int VK_LBUTTON_DOWN = 4001;
const int VK_LBUTTON_UP = 4002;
const int VK_RBUTTON_DOWN = 4003;
const int VK_RBUTTON_UP = 4004;
const int VK_MBUTTON_DOWN = 4005;
const int VK_MBUTTON_UP = 4006;

const int KINECT_TOP_ADD = 5001;
const int KINECT_TOP_DEC = 5002;
const int KINECT_LEFT_HAND_TOP = 5003;
const int KINECT_RIGHT_HAND_TOP = 5004;
const int KINECT_LEFT_HAND_FAST = 5005;
const int KINECT_RIGHT_HAND_FAST = 5006;

const int KINECT_BODY_FRONT = 5007;
const int KINECT_BODY_BACK = 5008;
const int KINECT_BODY_RIGHT = 5009;
const int KINECT_BODY_LEFT = 5010;
const int KINECT_WALK = 5011;

const int KINECT_TURN_RIGHT = 5012;
const int KINECT_TURN_LEFT = 5013;

const int LEAP_LEFT_CLOSED = 7000;
const int LEAP_RIGHT_CLOSED = 7001;
const int LEAP_LEFT_PINCH = 7002;
const int LEAP_RIGHT_PINCH = 7003;

const int LEAP_LEFT_FIST_UP = 7004;
const int LEAP_RIGHT_FIST_UP = 7005;
const int LEAP_LEFT_FIST_DOWN = 7006;
const int LEAP_RIGHT_FIST_DOWN = 7007;


const int MESSAGE = 6000;
const int ALERT = 6001;

class KeyMap {
private:
	std::string dev;

	std::string keyRepr;
	std::string toKeyRepr;

	int key;
	int toKey;
	int showMsg = 0;
	bool toKeyIsConstant = false;
	bool determineCenterPos = false;
	bool btnDown = true;
	bool btnUp = true;
	bool isBtn = false;

	bool waitingLeave = false;
	bool isLeaving = false;
	bool hasOnLeave = false;
	KeyMap *onLeave;

	int x = 0;
	int y = 0;

	//sensibilidade para mudanca de altura
	double heightSens = 0.15;

	std::string msg;

	int handTopMod = 0;// 0 =, 1 >, -1 <
	int handTopLevel = -100;
	int handXPos = -100;

	void init(std::string dev, char key[SSIZE], char toKey[SSIZE]);

public:

	
	int scan2ascii(DWORD scancode, USHORT* result);

	std::string getKeyRepr() {
		return keyRepr;
	}

	std::string getToKeyRepr() {
		return toKeyRepr;
	}

	std::string toString();

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}


	int getKey() {
		return key;
	}

	int getToKey() {
		return toKey;
	}

	std::string getDev() {
		return dev;
	}

	bool getToKeyIsConstant() {
		return toKeyIsConstant;
	}

	bool getIsBtn() {
		return isBtn;
	}

	bool getBtnDown() {
		return btnDown;
	}

	bool getBtnUp() {
		return btnUp;
	}

	bool getHasOnLeave() {
		return hasOnLeave;
	}

	bool getIsLeaving() {
		return isLeaving;
	}

	KeyMap* getOnLeave() {
		return onLeave;
	}

	bool getWaitingLeave() {
		return waitingLeave;
	}

	void setWaitingLeave(bool wait) {
		waitingLeave = wait;
	}

	double getHeightSens() {
		return heightSens;
	}

	int getHandTopMod() {
		return handTopMod;
	}

	int getHandTopLevel() {
		return handTopLevel;
	}

	int getHandXPos() {
		return handXPos;
	}

	int getShowMsg() {
		return showMsg;
	}

	std::string getMsg() {
		return msg;
	}

	bool getDetermineCenterPos() {
		return determineCenterPos;
	}

	KeyMap(){}
	KeyMap(std::string dev, int key, char toKey);
	KeyMap(std::string dev, std::string config);



	static  std::map<std::string, int> configToAscii;
	static  std::map<std::string, int> configToScanCode;
	
	static std::map<std::string, int> create_configToAscii() {
		//Caso o mapa ja tenha sido criado, nao cria novamente
		if ( configToAscii.size() > 0 ) {
			return configToAscii;
		}


		std::map < std::string, int > m;

		m["VK_ESCAPE"] = VK_ESCAPE;
		m["VK_UP"] = VK_UP;
		m["VK_DOWN"] = VK_DOWN;
		m["VK_LEFT"] = VK_LEFT;
		m["VK_RIGHT"] = VK_RIGHT;
		m["VK_BACK"] = VK_BACK;
		m["VK_TAB"] = VK_TAB;
		m["VK_RETURN"] = VK_RETURN;
		m["VK_CONTROL"] = VK_CONTROL;

		m["VK_LSHIFT"] = VK_LSHIFT;
		m["VK_RSHIFT"] = VK_RSHIFT;
		m["VK_MENU"] = VK_MENU;
		m["VK_SPACE"] = VK_SPACE;
		m["VK_CAPITAL"] = VK_CAPITAL;
		m["VK_F1"] = VK_F1;
		m["VK_F2"] = VK_F2;
		m["VK_F3"] = VK_F3;
		m["VK_F4"] = VK_F4;
		m["VK_F5"] = VK_F5;
		m["VK_F6"] = VK_F6;
		m["VK_F7"] = VK_F7;
		m["VK_F8"] = VK_F8;
		m["VK_F9"] = VK_F9;
		m["VK_F10"] = VK_F10;
		m["VK_F11"] = VK_F11;
		m["VK_F12"] = VK_F12;
		m["VK_NUMLOCK"] = VK_NUMLOCK;
		m["VK_SCROLL"] = VK_SCROLL;
		m["VK_HOME"] = VK_HOME;
		m["VK_UP"] = VK_UP;
		m["VK_PRIOR"] = VK_PRIOR;
		m["VK_LEFT"] = VK_LEFT;
		m["VK_CENTER"] = 5;
		m["VK_RIGHT"] = VK_RIGHT;
		m["VK_END"] = VK_END;
		m["VK_DOWN"] = VK_DOWN;
		m["VK_NEXT"] = VK_NEXT;
		m["VK_INSERT"] = VK_INSERT;
		m["VK_DELETE"] = VK_DELETE;
		m["VK_SNAPSHOT"] = VK_SNAPSHOT;
		m["VK_LWIN"] = VK_LWIN;
		m["VK_RWIN"] = VK_RWIN;
		m["VK_RMENU"] = VK_RMENU;
		m["VK_LAUNCH_MAIL"] = VK_LAUNCH_MAIL;


		//nao e possivel gerar eventos de mouse ainda
		m["VK_LBUTTON"] = VK_LBUTTON;
		m["VK_MBUTTON"] = VK_MBUTTON;
		m["VK_RBUTTON"] = VK_RBUTTON;
		m["VK_LBUTTON_DOWN"] = VK_LBUTTON_DOWN;
		m["VK_LBUTTON_UP"] = VK_LBUTTON_UP;
		m["VK_RBUTTON_DOWN"] = VK_RBUTTON_DOWN;
		m["VK_RBUTTON_UP"] = VK_RBUTTON_UP;
		m["VK_MBUTTON_DOWN"] = VK_MBUTTON_DOWN;
		m["VK_MBUTTON_UP"] = VK_MBUTTON_UP;

		
		return m;
	}

	static std::map<std::string, int> create_configToScanCode() {

		if ( configToScanCode.size() > 0 ) {
			return configToScanCode;
		}
		std::map < std::string, int > m;


		//MOUSE
		m["VK_LBUTTON"] = 0;
		m["VK_MBUTTON"] = 1;
		m["VK_RBUTTON"] = 2;

		//Kinect
		m["KINECT_TOP_ADD"] = KINECT_TOP_ADD;
		m["KINECT_TOP_DEC"] = KINECT_TOP_DEC;
		m["KINECT_LEFT_HAND_TOP"] = KINECT_LEFT_HAND_TOP;
		m["KINECT_RIGHT_HAND_TOP"] = KINECT_RIGHT_HAND_TOP;
		m["KINECT_LEFT_HAND_FAST"] = KINECT_LEFT_HAND_FAST;
		m["KINECT_RIGHT_HAND_FAST"] = KINECT_RIGHT_HAND_FAST;

		m["KINECT_BODY_FRONT"] = KINECT_BODY_FRONT;
		m["KINECT_BODY_BACK"] = KINECT_BODY_BACK;
		m["KINECT_BODY_RIGHT"] = KINECT_BODY_RIGHT;
		m["KINECT_BODY_LEFT"] = KINECT_BODY_LEFT;
		m["KINECT_WALK"] = KINECT_WALK;
		m["KINECT_TURN_RIGHT"] = KINECT_TURN_RIGHT;
		m["KINECT_TURN_LEFT"] = KINECT_TURN_LEFT;

		//Leap Motion
		m["LEAP_LEFT_CLOSED"] = LEAP_LEFT_CLOSED;
		m["LEAP_RIGHT_CLOSED"] = LEAP_RIGHT_CLOSED;
		m["LEAP_LEFT_PINCH"] = LEAP_LEFT_PINCH;
		m["LEAP_RIGHT_PINCH"] = LEAP_RIGHT_PINCH;
		m["LEAP_LEFT_FIST_UP"] = LEAP_LEFT_FIST_UP;
		m["LEAP_RIGHT_FIST_UP"] = LEAP_RIGHT_FIST_UP;
		m["LEAP_LEFT_FIST_DOWN"] = LEAP_LEFT_FIST_DOWN;
		m["LEAP_RIGHT_FIST_DOWN"] = LEAP_RIGHT_FIST_DOWN;


		//Mapeamento para ABNT2
		m["VK_ESCAPE"] = 1;
		m["1"] = 2;
		m["2"] = 3;
		m["3"] = 4;
		m["4"] = 5;
		m["5"] = 6;
		m["6"] = 7;
		m["7"] = 8;
		m["8"] = 9;
		m["9"] = 10;
		m["0"] = 11;
		m["_"] = 12;//BOTAO 12 E o unico que nao compreende o caracter natural, tem que usar _ em vez de -
		m["="] = 13;
		m["VK_BACK"] = 14;//CONFIRMAR SE E BACKSPACE
		m["VK_TAB"] = 15;
		m["Q"] = 16;
		m["W"] = 17;
		m["E"] = 18;
		m["R"] = 19;
		m["T"] = 20;
		m["Y"] = 21;
		m["U"] = 22;
		m["I"] = 23;
		m["O"] = 24;
		m["P"] = 25;
		m["´"] = 26;
		m["["] = 27;
		m["VK_RETURN"] = 28;//CONFIRMAR SE E ENTER
		m["VK_CONTROL"] = 29;
		m["A"] = 30;
		m["S"] = 31;
		m["D"] = 32;
		m["F"] = 33;
		m["G"] = 34;
		m["H"] = 35;
		m["J"] = 36;
		m["K"] = 37;
		m["L"] = 38;
		m["Ç"] = 39;
		m["~"] = 40;
		m["'"] = 41;
		m["VK_LSHIFT"] = 42;
		m["]"] = 43;
		m["Z"] = 44;
		m["X"] = 45;
		m["C"] = 46;
		m["V"] = 47;
		m["B"] = 48;
		m["N"] = 49;
		m["M"] = 50;
		m[","] = 51;
		m["."] = 52;
		m[";"] = 53;
		m["VK_RSHIFT"] = 54;

		m["*"] = 55;
		
		m["VK_MENU"] = 56;//ALT
		m["VK_SPACE"] = 57;
		
		m["VK_CAPITAL"] = 58;//CAPS

		m["VK_F1"] = 59;
		m["VK_F2"] = 60;
		m["VK_F3"] = 61;
		m["VK_F4"] = 62;
		m["VK_F5"] = 63;
		m["VK_F6"] = 64;
		m["VK_F7"] = 65;
		m["VK_F8"] = 66;
		m["VK_F9"] = 67;
		m["VK_F10"] = 68;
		
		m["VK_NUMLOCK"] = 69;
		m["VK_SCROLL"] = 70;
		m["VK_HOME"] = 71;
		m["VK_UP"] = 72;
		m["VK_PRIOR"] = 73;
		m["-"] = 74;
		m["VK_LEFT"] = 75;
		m["VK_CENTER"] = 76; //[VK_CENTER] nao existe
		m["VK_RIGHT"] = 77;
		m["+"] = 78;
		m["VK_END"] = 79;
		m["VK_DOWN"] = 80;
		m["VK_NEXT"] = 81;
		m["VK_INSERT"] = 82;
		m["VK_DELETE"] = 83;

		m["VK_SNAPSHOT"] = 84;

		m["\\"] = 86;

		m["VK_F11"] = 87;
		m["VK_F12"] = 88;
		m["VK_LWIN"] = 91;
		m["VK_RWIN"] = 92;
		m["VK_RMENU"] = 93;//tecla menu do teclado numerico

		m["VK_LAUNCH_MAIL"] = 108;
		m["/"] = 115;

		m["."] = 126;

		return m;
	}
	

};
