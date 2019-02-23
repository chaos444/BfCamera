#include "fonctions.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <stdlib.h>
#include <psapi.h>
#include <tchar.h>


namespace sigex
{
	uintptr_t ResolveEx(HANDLE hProcess, uintptr_t address, size_t offset = 0)
	{
		address += offset;

		BYTE instcode;
		ReadProcessMemory(hProcess, (void*)(address), &instcode, sizeof(instcode), NULL);

		if (instcode == 0xE9 || instcode == 0xE8) //e8 c6fbffff               CALL 0x1444ba9c0
		{
			int displacement;
			ReadProcessMemory(hProcess, (void*)(address + 1), &displacement, sizeof(displacement), NULL);

			auto ret = (__int64)address + displacement + 1 + sizeof(int32_t);
			return ret;
		}
		else // LEA
		{

			int displacement;

			ReadProcessMemory(hProcess, (void*)(address + 3), &displacement, sizeof(displacement), NULL);

			auto ret = (__int64)address + displacement + 3 + sizeof(int32_t);
			return ret;
		}
	}



	uintptr_t subFindPatternEx(char* base, size_t size, const char* pattern, const char *mask)
	{
		size_t patternLength = strlen(mask);

		for (size_t i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (size_t j = 0; j < patternLength; j++)
			{
				__int8 pat = pattern[j];
				__int8 mem = *(char*)(base + i + j);

				if (mask[j] != '?' && pattern[j] != *(char*)(base + i + j))
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return (uintptr_t)base + i;
			}
		}
		return 0;
	}


	/*
		const char * sigGame = "\x48\x8B\x05\x00\x00\x00\x00\x31\xD2\x48\x85\xC0\x74";
		const char * maskGame = "xxx????xxxxxx";

		auto firsttypeinfocode = FindPatternEx(hHandle, base, 0x000000014CFE9FFF, sigGame, maskGame);
		auto pfirsttypeinfo = Resolve(hHandle, firsttypeinfocode, 0);
	*/
	uintptr_t FindPatternEx(HANDLE hProcess, uintptr_t start, uintptr_t end, const char *pattern, const char *mask)
	{
		uintptr_t currentChunk = start;
		SIZE_T bytesRead;

		while (currentChunk < end)
		{
			byte buffer[4096];
			ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, 4096, &bytesRead);
			if (bytesRead == 0)
			{
				return 0;
			}
			uintptr_t InternalAddress = subFindPatternEx((char*)&buffer, bytesRead, pattern, mask);
			if (InternalAddress != 0)
			{
				uintptr_t offsetFromBuffer = InternalAddress - (uintptr_t)&buffer;
				return currentChunk + offsetFromBuffer;
			}
			else
			{
				currentChunk = currentChunk + bytesRead;
			}
		}
		return 0;
	}
}


namespace sig
{

	//
	// http://www.unknowncheats.me/forum/c-and-c/77419-findpattern.html#post650040
	// Original code by learn_more
#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))



	inline bool isMatch(const char*  addr, const char*  pat, const char*  msk)
	{
		size_t n = 0;
		while (addr[n] == pat[n] || msk[n] == '?') {
			if (!msk[++n]) {
				return true;
			}
		}
		return false;
	}

	size_t findPattern_v2(size_t rangeStart, size_t len, const char* pattern)
	{
		auto l = strlen(pattern);

		auto pat = new char[l];
		auto msk = new char[l];


		auto ps = 0;

		for (auto p = 0; p < l;)
		{
			if (pattern[p] == '?')
			{
				pat[ps] = 0;
				msk[ps] = '?';
				p = p + 2;
			}
			else
			{
				pat[ps] = getBits(pattern[p]) << 4 | getBits(pattern[p + 1]);
				msk[ps] = 'x';
				p = p + 3;
			}
			ps++;
		}

		msk[ps] = 0;


		size_t result = NULL;
		for (size_t n = 0; n < (len - l); ++n)
		{
			if (isMatch(reinterpret_cast<const char*>(rangeStart + n), pat, msk))
			{
				result = (rangeStart + n);
				break;
			}
		}

		delete[] pat;
		delete[] msk;

		return result;
	}


	size_t Resolve(size_t address, size_t offset)
	{
		address += offset;

		if (*reinterpret_cast< BYTE* >(address) == 0xE9 || *reinterpret_cast< BYTE* >(address) == 0xE8) //e8 c6fbffff               CALL 0x1444ba9c0
		{
			auto displacement = *reinterpret_cast< int32_t* >(address + 1);
			auto ret = address + displacement + 1 + sizeof(int32_t);
			return ret;
		}
		else
		{
			auto displacement = *reinterpret_cast< int32_t* >(address + 3); // LEA
			auto ret = address + displacement + 3 + sizeof(int32_t);
			return ret;
		}
	}
}



namespace procinfo
{
	///////////////////////////////////////////////////////////////
	//Usage:
	//
	//DWORD dwBase, dwSize;
	//
	//To get DLL base and size
	//GetModuleAddressAndSize("CShell.dll", &dwBase, &dwSize);
	//To get exe base and size
	//GetModuleAddressAndSize(NULL, &dwBase, &dwSize)
	/////////////////////////////////////////////////////////////////
	void GetModuleAddressAndSize(const char* dllName, DWORD64* base, DWORD64* size, HMODULE ModuleHandle) //zoomgod http://www.unknowncheats.me/forum/633534-post5.html
	{
		MODULEINFO module;
		ZeroMemory(&module, sizeof(module));
		*base = 0;
		*size = 0;
		if (!ModuleHandle)
		{
			ModuleHandle = GetModuleHandleA(dllName);
		}
		if (GetModuleInformation(GetCurrentProcess(), ModuleHandle, &module, sizeof(module)))
		{
			*base = (DWORD64)module.lpBaseOfDll;
			*size = (DWORD64)module.SizeOfImage;
		}
	};

	DWORD GetProcessPidByName(std::string name)
	{
		DWORD pid = 0;

		// Create toolhelp snapshot.
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);

		// Walkthrough all processes.
		if (Process32First(snapshot, &process))
		{
			do
			{
				// Compare process.szExeFile based on format of name, i.e., trim file path
				// trim .exe if necessary, etc.
				if (std::string(process.szExeFile) == name)
				{
					pid = process.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process));
		}

		CloseHandle(snapshot);

		return pid;
	}



	HANDLE GetProcessByName(std::string name)
	{
		DWORD pid = GetProcessPidByName(name);

		if (pid != 0)
		{
			return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		}
		// Not found
		return NULL;
	}




	HMODULE GetModule(std::string name)
	{
		DWORD pid = GetProcessPidByName(name);

		if (pid == 0) return NULL;
		// Not found
		HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);


		HMODULE hMods[1024];
		HANDLE pHandle = processHandle;
		DWORD cbNeeded;
		unsigned int i;

		if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
		{
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
				{
					std::string wstrModName = szModName;
					//you will need to change this to the name of the exe of the foreign process
					std::string wstrModContain = name;
					if (wstrModName.find(wstrModContain) != std::string::npos)
					{
						CloseHandle(pHandle);
						return hMods[i];
					}
				}
			}
		}
		return nullptr;
	}


	size_t GetProcessMainModuleSize(std::string name)
	{
		DWORD pid = GetProcessPidByName(name);

		if (pid == 0) return NULL;
		// Not found

		HANDLE       hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		HMODULE      hModule = GetModule(name);
		MODULEINFO  modinfo;
		DWORD        cb = sizeof(modinfo);

		auto success = GetModuleInformation(hProcess, hModule, &modinfo, cb);


		return modinfo.SizeOfImage;
	};


	uintptr_t GetProcessBaseAddress(std::string name)
	{
		DWORD pid = GetProcessPidByName(name);

		if (pid == 0) return NULL;
		// Not found

		HANDLE       hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		HMODULE      hModule = GetModule(name);
		MODULEINFO  modinfo;
		DWORD        cb = sizeof(modinfo);

		auto success = GetModuleInformation(hProcess, hModule, &modinfo, cb);


		return (uintptr_t)modinfo.lpBaseOfDll;
	}
}

void dumpsig()
{
	DWORD64 dwBase, dwSize;
	procinfo::GetModuleAddressAndSize(NULL, &dwBase, &dwSize,NULL);

}