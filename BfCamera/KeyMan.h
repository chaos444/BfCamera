#pragma once
#include <Windows.h>
#include <ctime>
#include <string>

struct KeyDef {
	std::string name;
	unsigned int keyCode;
};

class Keys {
public:
	static KeyDef showMenuKey;
	static KeyDef enableFreeCam;
	static KeyDef saveCameraLocation;
	static KeyDef recordCameraTrack;
	static KeyDef speedUpCamera;
	static KeyDef slowDownCamera;
	static KeyDef cameraForward;
	static KeyDef cameraBack;
	static KeyDef cameraLeft;
	static KeyDef cameraRight;
	static KeyDef camerUp;
	static KeyDef cameraDown;
};

class KeyMan {
private:
	static time_t lastTime;
public:
	// read key with a minimum time between keypresses
	static bool ReadKeyOnce(unsigned int keyCode, unsigned int timer = 250);
	static bool ReadKeyOnce(KeyDef keyDef, unsigned int timer = 250);

	// read a key without the time delay
	static bool ReadKey(unsigned int keyCode);
	static bool ReadKey(KeyDef keyDef);

	static bool windowActive;
	static bool requireActive;
};