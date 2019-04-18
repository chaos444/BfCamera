#include "Hooking/d3d11hook.h"
#include "ImGui/imgui.h"
#include "Menu.h"
#include <iostream>
#include "Discord/discord_rpc.h"

extern Matrix4x4 g_OldMatrix;
extern bool g_EnableFreeCam;
extern bool g_EnableMouseCam;

class CameraUpdate {
public:
	Matrix4x4 cameraTransform; //0x0000
	char pad_0040[168]; //0x0040
	bool b1; //0x00E8
	bool b2; //0x00E9
	bool b3; //0x00EA
	bool b4; //0x00EB
	bool b5; //0x00EC
	bool b6; //0x00ED
	bool b7; //0x00EE

};

typedef __int64(__fastcall*CAMERAUPDATE)(RenderView*, CameraUpdate*);
CAMERAUPDATE oCameraUpdate = nullptr;

__int64 hkCameraUpdate(RenderView* pRenderView, CameraUpdate* pMatrix) {
	if (g_EnableFreeCam) {
		if (g_EnableMouseCam) {
			pMatrix->cameraTransform._41 = g_OldMatrix._41;
			pMatrix->cameraTransform._42 = g_OldMatrix._42;
			pMatrix->cameraTransform._43 = g_OldMatrix._43;
		}
		else {
			pMatrix->cameraTransform = g_OldMatrix;
		}
	}	
	return oCameraUpdate(pRenderView, pMatrix);
}


time_t starttime = time(0);

DiscordRichPresence GetUpdate(const char* data, const char* playername, const char* faction, const char* image, const char* GameMode) {

	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = GameMode;
	discordPresence.details = playername;
	discordPresence.largeImageKey = image;
	discordPresence.largeImageText = faction;
	discordPresence.smallImageText = data;
	discordPresence.instance = 0;
	return discordPresence;
}

// main thread of the tool

DWORD WINAPI mainThread(LPVOID lpParam) {
	// allocate a console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);


	// pattern scan our offsets
	std::cout << std::hex << "GameRenderer:\t0x" << StaticOffsets::Get_OFFSET_GAMERENDERER() << std::endl;
	std::cout << std::hex << "FirstTypeInfo:\t0x" << StaticOffsets::Get_OFFSET_FIRSTTYPEINFO() << std::endl;
	std::cout << std::hex << "CamerHook:\t0x" << StaticOffsets::Get_OFFSET_CAMERAHOOK() << std::endl;
	std::cout << std::hex << "GameTimeSettings:\t0x" << StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() << std::endl;
	Wendy::SetHook((void*)StaticOffsets::Get_OFFSET_CAMERAHOOK(), &hkCameraUpdate, &oCameraUpdate);
	Discord_Initialize("551652329020063744", NULL, true, NULL);
	Discord_UpdatePresence(&GetUpdate("OpenGameCamera", "Testing Stuff", "Testing Stuff", "default", "\0"));

	std::cout << std::endl << std::endl;
	std::cout << "[" << Keys::showMenuKey.name << "]\t\tShow/Hide Menu" << std::endl;
	std::cout << "[" << Keys::enableFreeCam.name << "]\t\tEnable Camera" << std::endl;
	std::cout << "[" << Keys::saveCameraLocation.name << "]\t\tSave Camera" << std::endl;
	std::cout << "[" << Keys::recordCameraTrack.name << "]\t\tRecord Track" << std::endl;
	std::cout << std::endl;
	std::cout << "[" << Keys::cameraForward.name << "]\t\tCamera Forwards" << std::endl;
	std::cout << "[" << Keys::cameraBack.name << "]\t\tCamera Backwards" << std::endl;
	std::cout << "[" << Keys::cameraLeft.name << "]\t\tCamera Left" << std::endl;
	std::cout << "[" << Keys::cameraRight.name << "]\t\tCamera Right" << std::endl;
	std::cout << "[" << Keys::camerUp.name << "]\t\tCamera Up" << std::endl;
	std::cout << "[" << Keys::cameraDown.name << "]\t\tCamera Down" << std::endl;
	std::cout << "[" << Keys::speedUpCamera.name << "]\t\tCamera Speed Up" << std::endl;
	std::cout << "[" << Keys::slowDownCamera.name << "]\t\tCamera Slow Down" << std::endl;



	ImplHookDX11_Init((HMODULE)lpParam, FindWindow(0, "star wars battlefront ii"));

	// eject the module
	//FreeLibraryAndExitThread((HMODULE)lpParam, 1);
	return 1;
}

// the DllMain, the entry point of our module
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	// check if the dll was attached to a process
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		// create a thread to set up our stuff.  Pass the handle to our module for ejection
		CreateThread(NULL, NULL, mainThread, hModule, NULL, NULL);
	}

	// check if the dll is being unloaded
	if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		// need to unhook
		FreeConsole();
	//	Wendy::UnHookAll();
	}
	return TRUE;
}
