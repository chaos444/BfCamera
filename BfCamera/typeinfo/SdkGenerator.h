#pragma once


#include <vector>
#include <algorithm>



#include "Sdk.h"

#include "SdkPrimitive.h"
#include "SdkClass.h"
#include "SdkEnum.h"
#include "SdkStruct.h"

namespace Backend
{
	class SDK_GENERATOR
	{
	private:
		std::vector<fb::PrimitiveTypeInfo*>* m_primitiveInfos;
		std::vector<fb::EntityCreator*>* m_entityCreators;
		std::vector<fb::TypeInfo*>* m_typeInfos;
		std::vector<fb::ClassInfo*>* m_classInfos;
		std::vector<fb::EnumTypeInfo*>* m_enumInfos;
		std::vector<fb::ValueTypeInfo*>* m_valueInfos;
		std::vector<PSDK_PRIMITIVE>* m_sdkPrimitives;
		std::vector<PSDK_CLASS>* m_sdkClasses;
		std::vector<PSDK_ENUM>* m_sdkEnums;
		std::vector<PSDK_STRUCT>* m_sdkStructs;


		DWORD64 m_dwBase, m_dwSize;

		std::string m_prefix;
		bool m_isforidaimport;

	public:
		SDK_GENERATOR(DWORD64 dwBase, DWORD64 dwSize, std::string prefix, bool isforidaimport);
		virtual ~SDK_GENERATOR();

		void RegisterType(fb::TypeInfo* typeInfo);
		void RegisterCreator(fb::EntityCreator* creator);
		void Analyze();
		void Generate(std::vector<std::string>& result);

		void GenerateDeclarations(std::vector<std::string>& result);
		void GeneratePrimitives(std::vector<std::string>& result);
		void GenerateClasses(std::vector<std::string>& result);
		void GenerateEnums(std::vector<std::string>& result);
		void GenerateStructs(std::vector<std::string>& result);

		void GenerateIDAClasses(std::vector<std::string>& result);
		void GenerateIDAEnums(std::vector<std::string>& result);
		void GenerateIDAStructs(std::vector<std::string>& result);

		void GenerateIDAGetTypeClasses(std::vector<std::string>& result);
		void GenerateIDAGetTypeStructs(std::vector<std::string>& result);


		void savetofile(std::string filename, std::vector<std::string> &results);

	private:
		void AnalyzePrimitives();
		void AnalyzeClasses();
		void AnalyzeEnums();
		void AnalyzeStructs();

		std::string GetTypeName(fb::TypeInfo* typeInfo);
		unsigned int GetTypeSize(fb::TypeInfo* typeInfo);
		void GetDependenciesForStructMember(fb::TypeInfo* typeInfo, std::vector<PSDK_STRUCT>& dependencies);

	};
	typedef SDK_GENERATOR* PSDK_GENERATOR;
}