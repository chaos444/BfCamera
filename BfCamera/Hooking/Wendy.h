#include <Windows.h>
#include <vector>
#include <algorithm>

// wendy is a hooker :)

class Wendy {
private:
	static std::vector<void*> hookAddresses;
public:
	static void SetHook(void* addr, void* callback, void* original);
	static void UnHook(void* addr);
	static void UnHookAll(void);
};