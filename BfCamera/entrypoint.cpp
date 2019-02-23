#include "SigScan/StaticOffsets.h"
#include "Hooking/Wendy.h"
#include "typeinfo/Util.h"
#include "fbsdk.h"

// main thread of the tool
DWORD WINAPI mainThread(LPVOID lpParam) {
	// allocate a console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	// pattern scan our offsets
	std::cout << std::hex << "GameRenderer:\t0x" << StaticOffsets::Get_OFFSET_GAMERENDERER() << std::endl;
	std::cout << std::hex << "FirstTypeInfo:\t0x" << StaticOffsets::Get_OFFSET_FIRSTTYPEINFO() << std::endl;

	// test print out the renderview address
	GameRenderer* pGameRenderer = *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();
	std::cout << std::hex << pGameRenderer->renderView << std::endl;

	// eject the module
	FreeLibraryAndExitThread((HMODULE)lpParam, 1);
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
		Wendy::UnHookAll();
	}
	return TRUE;
}
