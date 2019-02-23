#ifndef BF4FONC_H
#define BF4FONC_H

#include <Windows.h>
#include <Psapi.h>
#include <string>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _AMD64_
#define _MAX_VALUE ((DWORD64)0x000F000000000000)
#define _VALUE ULONG_PTR
#define _Allign 0x7 // soit 0x...0 ou 0x...8
#else
#define _MAX_VALUE ((PVOID)0xFFF00000)
#define _VALUE ULONG
#define _Allign 0x3
#endif

__forceinline bool IsAligned(DWORD64 p) { return ((_VALUE)p & _Allign); }
__forceinline bool IsValidPtr(DWORD64 p) { return (p >= (DWORD64)0x10000) && (p < _MAX_VALUE) && !((_VALUE)p & _Allign); }
__forceinline bool IsValidDataPtr(DWORD64 p) { return (p >= (DWORD64)0x10000) && (p < _MAX_VALUE); }


namespace procinfo
{
	void GetModuleAddressAndSize(const char* dllName, DWORD64* base, DWORD64* size, HMODULE ModuleHandle);
	HANDLE GetProcessByName(std::string name);
	DWORD GetProcessPidByName(std::string name);
	size_t GetProcessMainModuleSize(std::string name);
	uintptr_t GetProcessBaseAddress(std::string name);

}

namespace sig
{
	size_t findPattern_v2(size_t rangeStart, size_t len, const char* pattern);
	size_t Resolve(size_t address, size_t offset = 0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dumpsig();


#endif