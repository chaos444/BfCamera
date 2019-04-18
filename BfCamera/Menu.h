#pragma once
#include "KeyMan.h"
#include "Renderer.h"
#include "SigScan/StaticOffsets.h"
#include "fbsdk.h"
#include "Hooking/Wendy.h"

class Menu
{
public:
	bool IsOpen = true;
	void Render();
	void ToggleMenu();
};
