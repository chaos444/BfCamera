#include "fonctions.h"
#include "SdkGenerator.h"
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

bool IsValidPtr(void* ptr) {
	return !IsBadReadPtr(ptr, sizeof(LPVOID));
}

namespace fb {

	class MemberInfoData
	{
	public:
		const char* m_name; //0x0000
	};
}

std::string GetTypeName(fb::TypeInfo* typeInfo)
{
	switch (typeInfo->getTypeCode())
	{
	case fb::kTypeCode_ValueType:
		return typeInfo->getTypeInfoData()->name;
	case fb::kTypeCode_Class:
		return std::string(typeInfo->getTypeInfoData()->name) + "*";
	case fb::kTypeCode_Array:
	case fb::kTypeCode_FixedArray:
	{
		fb::ArrayTypeInfo* arrayTypeInfo = (fb::ArrayTypeInfo*) typeInfo;
		fb::ArrayTypeInfo::ArrayTypeInfoData* arrayTypeInfoData = arrayTypeInfo->getArrayTypeInfoData();
		//if (!m_isforidaimport)
		//	return std::string("Array<") + m_prefix + GetTypeName(arrayTypeInfoData->elementType) + ">";
		//else
		return std::string(GetTypeName(arrayTypeInfoData->elementType) + " * ");
	}
	case fb::kTypeCode_Enum:
		return typeInfo->getTypeInfoData()->name;

	case fb::kTypeCode_CString:
	case fb::kTypeCode_Boolean:
	case fb::kTypeCode_Int8:
	case fb::kTypeCode_Uint8:
	case fb::kTypeCode_Int16:
	case fb::kTypeCode_Uint16:
	case fb::kTypeCode_Int32:
	case fb::kTypeCode_Uint32:
	case fb::kTypeCode_Int64:
	case fb::kTypeCode_Uint64:
	case fb::kTypeCode_Float32:
	case fb::kTypeCode_Float64:
	case fb::kTypeCode_Guid:
	case fb::kTypeCode_Void:
	case fb::kTypeCode_DbObject:
	case fb::kTypeCode_String:
	case fb::kTypeCode_FileRef:
	case fb::kTypeCode_SHA1:
	case fb::kTypeCode_ResourceRef:
	{
		fb::PrimitiveTypeInfo* primInfo = (fb::PrimitiveTypeInfo*)typeInfo;
		fb::PrimitiveTypeInfo::PrimitiveTypeInfoData * primInfoData = primInfo->getPrimitiveTypeInfoData();
		return primInfoData->tid.name;
	}
	default:
	{
		//LOG_DEBUG << "// unhandled basic type name " << std::string(typeInfo->getName());
		return "// unhandled basic type " + std::string(typeInfo->getName());
	}
	}
}


struct Setting {
	void* classPtr;
	fb::TypeInfo* typeInfo;
};

std::vector<Setting> classes;

void* resolveJump(void* addr) {
	if (!IsValidPtr(addr)) return nullptr;
	if ((byte)addr != 0xe9) return nullptr;
	DWORD relAddress = (DWORD)((DWORD64)addr) + 1;
	DWORD64 ripAddress = (DWORD64)addr + 5;
	void* absAddress = (void*)(ripAddress + relAddress);
	if (!IsValidPtr(absAddress)) return nullptr;
	return absAddress;
}

void* resolveLea(void* addr) {
	if (!IsValidPtr(addr)) return nullptr;
	if ((byte)addr != 0x48) return nullptr;
	DWORD relAddress = (DWORD)((DWORD64)addr) + 3;
	DWORD64 ripAddress = (DWORD64)addr + 7;
	void* absAddress = (void*)(ripAddress + relAddress);
	if (!IsValidPtr(absAddress)) return nullptr;
	return absAddress;
}

fb::TypeInfo* GetTypeManual(void* _this) {
	if (!IsValidPtr(_this) || !IsValidPtr(*(void**)_this) || !IsValidPtr(**(void***)_this)) return nullptr;
	void* firstVtable = **(void***)_this;
	void* typeInfoPtr = firstVtable;
	if ((byte)typeInfoPtr == 0xe9) {
		typeInfoPtr = resolveJump(typeInfoPtr);
		if (!IsValidPtr(typeInfoPtr)) return nullptr;
	}
	typeInfoPtr = resolveLea(typeInfoPtr);
	if (!IsValidPtr(typeInfoPtr)) return nullptr;
	return (fb::TypeInfo*)typeInfoPtr;
}

struct typeInfoMemberResult {
	void* pVTable;
	const char* name;
	DWORD offset;
};

std::vector<typeInfoMemberResult> typeInfoMemberResults;

template <class T = void*>
T GetClassFromNonDumpable(void* addr, const char* name, SIZE_T classSize = 0x2048, bool rescan=false) {
	if (!rescan) {
		for (typeInfoMemberResult &result : typeInfoMemberResults) {
			if (result.pVTable == *(void**)addr) {
				if (result.name == name) {
					return *(T*)((DWORD64)addr + result.offset);
				}
			}
		}
	}

	const byte INSTR_LEA = 0x48;
	const byte INSTR_RET = 0xc3;
	const byte INSTR_JMP = 0xe9;
	const DWORD64 BASE_ADDRESS = 0x140000000;
	const DWORD64 MAX_ADDRESS = 0x14fffffff;

	DWORD offset = 0;
	DWORD lastOffset = 0;
	while (offset < classSize) {
		offset += 8;
		if (!IsValidPtr((void*)((DWORD64)addr + offset))) continue;
		void* czech = *(void**)((DWORD64)addr + offset);
		if (!IsValidPtr(czech)) continue;
		if (!IsValidPtr(*(void**)czech)) continue; // vtable
		if (!IsValidPtr(**(void***)czech)) continue; // virtual 1;
		void* pGetType = **(DWORD64***)czech;
		if ((DWORD64)pGetType < BASE_ADDRESS || (DWORD64)pGetType > MAX_ADDRESS) continue;

		if (*(byte*)pGetType == INSTR_JMP || *(byte*)pGetType == INSTR_LEA) {
			void* pTypeInfo = nullptr;
			if (*(byte*)pGetType == INSTR_JMP) {
				std::cout << std::hex << "rel:\t" << *(int32_t*)((DWORD64)pGetType + 1) << "\tRIP:\t" << (DWORD64)pGetType + 5 << std::endl;
				pGetType = (void*)(*(int32_t*)((DWORD64)pGetType + 1) + (DWORD64)pGetType + 5);
			}
			if (*(byte*)pGetType == INSTR_LEA) {
				if (*(byte*)((DWORD64)pGetType + 7) != INSTR_RET) continue;
				pTypeInfo = (void*)(*(int32_t*)((DWORD64)pGetType + 3) + (DWORD64)pGetType + 7);
			}
			else continue;
			if (!IsValidPtr(pTypeInfo)) continue;
			void* pMemberInfo = *(void**)pTypeInfo;
			if (!IsValidPtr(pMemberInfo)) continue;
			char* m_name = *(char**)pMemberInfo;
			if (!IsValidPtr(m_name)) continue;
			if ((DWORD64)pTypeInfo > BASE_ADDRESS && (DWORD64)pTypeInfo < MAX_ADDRESS) {
				if (strcmp(m_name, name) == 0) {
					typeInfoMemberResult result;
					result.name = name;
					result.offset = offset;
					result.pVTable = *(void**)addr;
					typeInfoMemberResults.push_back(result);
					return *(T*)((DWORD64)addr + offset);
				}

				lastOffset = offset;
			}
		}
	}
	return nullptr;
}



void DumpClass(void* addr, SIZE_T classSize = 0x2048) {
	std::cout  << std::hex << "dumping " << (DWORD_PTR)addr << std::endl;

	const byte INSTR_LEA = 0x48;
	const byte INSTR_RET = 0xc3;
	const byte INSTR_JMP = 0xe9;
	const DWORD64 BASE_ADDRESS = 0x140000000;
	const DWORD64 MAX_ADDRESS = 0x14fffffff;

	DWORD offset = 0;
	DWORD lastOffset = 0;
	while (offset < classSize) {
		offset += 8;
		if (!IsValidPtr((void*)((DWORD64)addr + offset))) continue;
		void* czech = *(void**)((DWORD64)addr + offset);
		if (!IsValidPtr(czech)) continue;
		if (!IsValidPtr(*(void**)czech)) continue; // vtable
		if (!IsValidPtr(**(void***)czech)) continue; // virtual 1;
		void* pGetType = **(DWORD64***)czech;
		if ((DWORD64)pGetType < BASE_ADDRESS || (DWORD64)pGetType > MAX_ADDRESS) continue;

		if (*(byte*)pGetType == INSTR_JMP || *(byte*)pGetType == INSTR_LEA) {
			void* pTypeInfo = nullptr;
			if (*(byte*)pGetType == INSTR_JMP) {
			//	std::cout << std::hex << *(DWORD*)((DWORD64)pGetType + 1) << "\t" << (DWORD64)pGetType + 5 << std::endl;
				pGetType = (void*)(*(DWORD*)((DWORD64)pGetType + 1) + (DWORD64)pGetType + 5);
			}
			if (*(byte*)pGetType == INSTR_LEA) {
				if (*(byte*)((DWORD64)pGetType + 7) != INSTR_RET) continue;
				pTypeInfo = (fb::TypeInfo*)(*(DWORD*)((DWORD64)pGetType + 3) + (DWORD64)pGetType + 7 - 0x100000000);
			}
			else continue;
			void* pMemberInfo = *(void**)pTypeInfo;
			if (!IsValidPtr(pMemberInfo)) continue;
			char* m_name = *(char**)pMemberInfo;
			if (!IsValidPtr(m_name)) continue;
			if (!IsValidPtr(pTypeInfo)) continue;
			if ((DWORD64)pTypeInfo > BASE_ADDRESS && (DWORD64)pTypeInfo < MAX_ADDRESS) {
				std::cout << std::hex << "char pad_00" << offset << "[0x" << offset - lastOffset << "];\t// 0x" << offset << std::endl;
				std::cout << std::hex << m_name << "* m_" << m_name << ";\t// 0x" << offset << std::endl;

				lastOffset = offset;
			}
		}
	}
	std::cout << std::hex << "Done, last offset:\t0x" << offset << std::endl;
}

template <class T=void*>
T getTypeInfoMember(void* object, const char* name) {
	if (!IsValidPtr(object)) return nullptr;
	if (!IsValidPtr(*(void**)object)) return nullptr;

	for (typeInfoMemberResult &result : typeInfoMemberResults) {
		if (result.pVTable == *(void**)object) {
			if (result.name == name) {
				return (T)((DWORD64)object + result.offset);
			}
		}
	}

	fb::TypeInfo* pTypeInfo;
	fb::ITypedObject* pTypedObject = (fb::ITypedObject*)object;
	pTypeInfo = pTypedObject->GetType();
	if (!IsValidPtr(pTypeInfo)) return nullptr;
	fb::ClassInfo::TypeInfoData* classInfoData = (fb::ClassInfo::TypeInfoData*)pTypeInfo->getTypeInfoData();
	if (!IsValidPtr((void*)classInfoData->basicInfo.name)) return nullptr;
	fb::FieldInfo::FieldInfoData* fields = classInfoData->fields;

	for (int i = 0; i < classInfoData->basicInfo.fieldCount; i++)
	{
		fb::FieldInfo::FieldInfoData* fieldInfoData = &fields[i];
		if (!IsValidPtr(fieldInfoData)) continue;
		fb::TypeInfo* fieldType = fieldInfoData->fieldTypePtr;
		if (!IsValidPtr(fieldType)) continue;
		fb::TypeInfo::TypeInfoData* fieldTypeData = fieldType->getTypeInfoData();
		if (!IsValidPtr(fieldTypeData)) continue;
		if (!IsValidPtr((void*)fieldInfoData->name) || strlen(fieldInfoData->name) <= 0) continue;
		if (strcmp(fieldInfoData->name, name) == 0) {
			typeInfoMemberResult result;
			result.name = name;
			result.offset = fieldInfoData->fieldOffset;
			result.pVTable = *(void**)object;
			typeInfoMemberResults.push_back(result);
			return (T)((DWORD64)object + fieldInfoData->fieldOffset);
		}
	}
	return nullptr;
}

void DumpAllTypeInfoNames() {
	fb::TypeInfo* pTypeInfo = *(fb::TypeInfo**)StaticOffsets::Get_OFFSET_FIRSTTYPEINFO();
	while (IsValidPtr(pTypeInfo)) {
		std::cout << pTypeInfo->getName() << std::endl;
		if (!IsValidPtr(pTypeInfo->m_pNext)) return;
		pTypeInfo = pTypeInfo->m_pNext;
	}
}


void DumpTypeInfoByName(const char* name) {
	fb::TypeInfo* pTypeInfo = *(fb::TypeInfo**)StaticOffsets::Get_OFFSET_FIRSTTYPEINFO();
	while (strcmp(pTypeInfo->getName(), name) != 0) {
		if (!IsValidPtr(pTypeInfo->m_pNext)) return;
		pTypeInfo = pTypeInfo->m_pNext;
	}
	fb::ClassInfo::TypeInfoData* classInfoData = (fb::ClassInfo::TypeInfoData*)pTypeInfo->getTypeInfoData();
	if (!IsValidPtr((void*)classInfoData->basicInfo.name)) return;
	fb::FieldInfo::FieldInfoData* fields = classInfoData->fields;

	std::cout << "Class " << classInfoData->basicInfo.name << std::endl << "{" << std::endl;
	DWORD lastOffset = 0;
	for (int i = 0; i < classInfoData->basicInfo.fieldCount; i++)
	{
		fb::FieldInfo::FieldInfoData* fieldInfoData = &fields[i];
		if (!IsValidPtr(fieldInfoData)) continue;
		fb::TypeInfo* fieldType = fieldInfoData->fieldTypePtr;
		if (!IsValidPtr(fieldType)) continue;
		fb::TypeInfo::TypeInfoData* fieldTypeData = fieldType->getTypeInfoData();
		if (!IsValidPtr(fieldTypeData)) continue;
		if (!IsValidPtr((void*)fieldInfoData->name) || strlen(fieldInfoData->name) <= 0) continue;
		//std::cout << "\tchar pad_0" << fieldInfoData->fieldOffset << "[0x" << std::hex << fieldInfoData->fieldOffset - lastOffset << "]; // 0x" << fieldInfoData->fieldOffset << std::endl;
	
		std::cout << "\t" << fb::toString(fieldType->getTypeCode()) << " " << fieldInfoData->fieldTypePtr->getName() << " " << fieldInfoData->name << "; // 0x" << std::hex << fieldInfoData->fieldOffset << std::endl;
		lastOffset = fieldInfoData->fieldOffset;
	}
	std::cout << "};" << std::endl; 
}

void DumpTypeInfoClass(void* object) {
	if (!IsValidPtr(object)) return;
	if (!IsValidPtr(*(void**)object)) return;

	fb::TypeInfo* pTypeInfo;
	fb::ITypedObject* pTypedObject = (fb::ITypedObject*)object;
	pTypeInfo = pTypedObject->GetType();
	if (!IsValidPtr(pTypeInfo)) return;
	fb::ClassInfo::TypeInfoData* classInfoData = (fb::ClassInfo::TypeInfoData*)pTypeInfo->getTypeInfoData();
	if (!IsValidPtr((void*)classInfoData->basicInfo.name)) return;
	fb::FieldInfo::FieldInfoData* fields = classInfoData->fields;

	std::cout << "Class " << classInfoData->basicInfo.name << std::endl << "{" << std::endl;
	DWORD lastOffset = 0;
	for (int i = 0; i < classInfoData->basicInfo.fieldCount; i++)
	{
		fb::FieldInfo::FieldInfoData* fieldInfoData = &fields[i];
		if (!IsValidPtr(fieldInfoData)) continue;
		fb::TypeInfo* fieldType = fieldInfoData->fieldTypePtr;
		if (!IsValidPtr(fieldType)) continue;
		fb::TypeInfo::TypeInfoData* fieldTypeData = fieldType->getTypeInfoData();
		if (!IsValidPtr(fieldTypeData)) continue;
		if (!IsValidPtr((void*)fieldInfoData->name) || strlen(fieldInfoData->name) <= 0) continue;
		//std::cout << "\tchar pad_0" << fieldInfoData->fieldOffset << "[0x" << std::hex << fieldInfoData->fieldOffset - lastOffset << "]; // 0x" << fieldInfoData->fieldOffset << std::endl;
		std::cout << "\t" << fb::toString(fieldType->getTypeCode()) << " " << fieldInfoData->fieldTypePtr->getName() << " " << fieldInfoData->name << "; // 0x" << std::hex << fieldInfoData->fieldOffset << std::endl;
		lastOffset = fieldInfoData->fieldOffset;
	}
	std::cout << "};" << std::endl;
	return ;
}


template <class T=void*>
T TracePointer(void* addr, std::vector<const char*> names) {
	void* retObject = addr;
	for (auto &name : names) {
		auto checkPtr = getTypeInfoMember(retObject, name);
		if (!IsValidPtr(checkPtr)) return nullptr;
		retObject = checkPtr;
	}
	return (T)retObject;
}
