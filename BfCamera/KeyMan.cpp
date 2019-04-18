#include "KeyMan.h"

bool KeyMan::ReadKeyOnce(unsigned int keycode, unsigned int timer) {
	if (KeyMan::requireActive && !KeyMan::windowActive) return false;
	if ((clock() - lastTime > timer) && GetAsyncKeyState(keycode)) {
		KeyMan::lastTime = clock();
		return true;
	}
	return false;
}

bool KeyMan::ReadKeyOnce(KeyDef keyDef, unsigned int timer) {
	return KeyMan::ReadKeyOnce(keyDef.keyCode, timer);
}

bool KeyMan::ReadKey(unsigned int keycode) {
	if (KeyMan::requireActive && !KeyMan::windowActive) return false;
	return GetAsyncKeyState(keycode);
}

bool KeyMan::ReadKey(KeyDef keyDef) {
	return KeyMan::ReadKey(keyDef.keyCode);
}


bool KeyMan::windowActive = true;
bool KeyMan::requireActive = true;

time_t KeyMan::lastTime = clock();

KeyDef Keys::showMenuKey = { "HOME", VK_HOME };
KeyDef Keys::enableFreeCam = { "F5", VK_F5 };
KeyDef Keys::saveCameraLocation = { "F6", VK_F6 };
KeyDef Keys::recordCameraTrack = { "F7", VK_F7 };
KeyDef Keys::speedUpCamera = { "SHIFT", VK_SHIFT };
KeyDef Keys::slowDownCamera = { "ALT", VK_MENU };
KeyDef Keys::cameraForward = { "W", 0x57 };
KeyDef Keys::cameraBack = { "S", 0x53 };
KeyDef Keys::cameraLeft = { "A", 0x41 };
KeyDef Keys::cameraRight = { "D", 0x44 };
KeyDef Keys::camerUp = { "SPACE", VK_SPACE };
KeyDef Keys::cameraDown = { "CONTROL", VK_CONTROL };