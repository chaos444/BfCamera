#ifndef __FROSTBITGENERAL_H__
#define __FROSTBITGENERAL_H__



#include <Windows.h>


// TypeInfo: 0x14279FBE0 "FileRef" Size: 0x8
// Flags:0x409D Fields:8
struct FileRef
{
public:
	char _0x000[0x8];
};


// TypeInfo: 0x14279FB80 "ResourceRef" Size: 0x8
// Flags:0x417D Fields:8

struct ResourceRef
{
public:
	char _0x000[0x8];
};

// TypeInfo: 0x14279FBA0 "DbObject" Size: 0x10
// Flags:0x1D Fields:16

struct DbObject
{
public:
	char _0x000[0x10];
};

struct Guid
{
public:
	unsigned long m_Data1; //0x0000
	unsigned short m_Data2; //0x0004
	unsigned short m_Data3; //0x0006
	unsigned char m_Data4[8]; //0x0008
};//Size=0x0010


#include "Eastl.h"


namespace fb
{
	typedef eastl::allocator eastl_arena_allocator;
#define EASTLDummyAllocatorType

	
}



#endif