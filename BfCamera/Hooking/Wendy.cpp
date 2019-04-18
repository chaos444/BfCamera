#include "Wendy.h"

#define HOOKLIB_MINHOOK

#ifdef HOOKLIB_MINHOOK
#include "minhook/MinHook.h"
#endif

void Wendy::SetHook(void* addr, void* callback, void* original) {
#ifdef HOOKLIB_MINHOOK
	static bool firstTime = true;
	if (firstTime) {
		MH_Initialize();
		firstTime = false;
	}
#endif

	Wendy::hookAddresses.push_back(addr);

#ifdef HOOKLIB_MINHOOK
	MH_CreateHook(addr, callback, (LPVOID*)original);
	MH_EnableHook(addr);
#endif
}

void Wendy::UnHook(void* addr) {
	Wendy::hookAddresses.erase(remove(Wendy::hookAddresses.begin(), Wendy::hookAddresses.end(), addr), Wendy::hookAddresses.end());
#ifdef HOOKLIB_MINHOOK
	MH_DisableHook(addr);
#endif
}

void Wendy::UnHookAll(void) {
#ifdef HOOKLIB_MINHOOK
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
#else
	for (auto &addr : Wendy::hookAddresses) {
		Wendy::UnHook(addr);
	}
#endif
}

std::vector<void*> Wendy::hookAddresses = {};