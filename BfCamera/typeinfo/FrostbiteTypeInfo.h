#pragma once


#include <string>


#include "FrostbiteGeneral.h"


namespace fb
{
	class Noncopyable;
	struct MemoryArena;
	struct MemoryArenaVtbl;
	class ITypedObject;
	struct ITypedObjectVtbl;
	struct TestList;
	struct ModuleInfo;
	const struct TestDetails;
	struct TestReporter;
	struct TestReporterVtbl;
	class TypeInfo;
	struct Test;
	struct FieldInfoVtbl;
	class FieldInfo;
	struct ArrayTypeInfoVtbl;
	class ArrayTypeInfo;
	class ClassInfo;


	typedef unsigned short ClassId;

	class ITypedObject
	{
	public:
		virtual TypeInfo* GetType();

	};//Size=0x0008

	class DataContainer : public ITypedObject
	{

		union
		{
			struct
			{
				unsigned short m_refCnt; //0x0008
				unsigned short m_flags; //0x000A
			};
			__int32 m_refCountAndId; //0x0008
		};
		char _0x000C[4];
	};

	class Noncopyable
	{
		//~Noncopyable(void);

	};


	/*
	class DbVariant 
	{
		struct DbVariant::Core : public Noncopyable 
		{
		};
		const unsigned __int8 *m_data;
		DbVariant::Core *m_corePtr;
	};
	class DbObject : public DbVariant 
	{
	};


	class ResourceRef
	{
	protected:
		//todofb::ResourceRef::Content m_content;
		static unsigned __int64 kModeFlag;
		static const std::intptr_t kPtrMask;
	};
	*/

	enum BasicTypesEnum
	{
		kTypeCode_Void,
		kTypeCode_DbObject,
		kTypeCode_ValueType,
		kTypeCode_Class,
		kTypeCode_Array,
		kTypeCode_FixedArray,
		kTypeCode_String,
		kTypeCode_CString,
		kTypeCode_Enum,
		kTypeCode_FileRef,
		kTypeCode_Boolean,
		kTypeCode_Int8,
		kTypeCode_Uint8,
		kTypeCode_Int16,
		kTypeCode_Uint16,
		kTypeCode_Int32,
		kTypeCode_Uint32,
		kTypeCode_Int64,
		kTypeCode_Uint64,
		kTypeCode_Float32,
		kTypeCode_Float64,
		kTypeCode_Guid,
		kTypeCode_SHA1,
		kTypeCode_ResourceRef,
		kTypeCode_BasicTypeCount
	};

	const char * toString(BasicTypesEnum typeCode);
	const std::string toCTypeString(BasicTypesEnum typeCode);

	enum MemberTypeEnum
	{
		kMemberType_Field,
		kMemberType_TypeInfo
	};

	const char * toString(MemberTypeEnum memberType);

	enum TypeCategoryEnum
	{
		kTypeCategory_NotApplicable,
		kTypeCategory_Class,
		kTypeCategory_ValueType,
		kTypeCategory_PrimitiveType
	};

	const char * toString(TypeCategoryEnum typeCategory);

	const struct TraceRange
	{
		const char *m_rangeDebugName;
		unsigned int m_rangeIdentifier;
		unsigned int m_parentRange;
	};


	struct MemoryArenaVtbl
	{
		unsigned int(__thiscall *getBytesAllocated)(MemoryArena *);
		unsigned int(__thiscall *queryBlockSize)(MemoryArena *, void *);
		const char *(__thiscall *debugName)(MemoryArena *);
		void *(__thiscall *internalAlloc)(MemoryArena *, unsigned int, unsigned int);
		void(__thiscall *internalFree)(MemoryArena *, void *, unsigned int);
		TraceRange *(__thiscall *memoryPoolRange)(MemoryArena *);
	};


	struct MemoryArena
	{
		MemoryArenaVtbl *vfptr;
		const int m_flags;
		MemoryArena *m_front;
	};




	struct TestList
	{
		Test *m_head;
		Test *m_tail;
	};

	struct ModuleInfo
	{
		const char *m_moduleName;
		ModuleInfo *m_nextModule;
		TestList *m_testList;
	};


	const struct TestDetails
	{
		const char *const suiteName;
		const char *const testName;
		const char *const filename;
		const int lineNumber;
	};


	struct TestReporter
	{
		TestReporterVtbl *vfptr;
	};

	struct TestReporterVtbl
	{
		void *(__thiscall *__vecDelDtor)(TestReporter *, unsigned int);
		void(__thiscall *reportTestStart)(TestReporter *, TestDetails *);
		void(__thiscall *reportFailure)(TestReporter *, TestDetails *, const char *);
		void(__thiscall *reportTestFinish)(TestReporter *, TestDetails *, float);
		void(__thiscall *reportSummary)(TestReporter *, int, int, int, float);
	};

	struct __declspec(align(8)) TestResults
	{
		TestReporter *m_testReporter;
		int m_totalTestCount;
		int m_failedTestCount;
		int m_failureCount;
		bool m_currentTestFailed;
	};

	struct TestVtbl
	{
		void *(__thiscall *__vecDelDtor)(Test *, unsigned int);
		void(__thiscall *runImpl)(Test *, TestResults *);
	};

	struct __declspec(align(8)) Test
	{
		TestVtbl *vfptr;
		TestDetails m_details;
		Test *next;
		bool m_timeConstraintExempt;
	};




	struct MemberInfoFlags
	{
		unsigned __int16 flagBits;
		enum FlagsMask
		{
			kMemberTypeMask = 0x3,
			kTypeCategoryShift = 0x2,
			kTypeCategoryMask = 0x3,
			kTypeCodeShift = 0x4,
			kTypeCodeMask = 0x1F,
			kMetadata = 0x800,
			kHomogeneous = 0x1000,
			kAlwaysPersist = 0x2000,
			kExposed = 0x2000,
			kLayoutImmutable = 0x4000,
			kBlittable = 0x8000,
		};


		TypeCategoryEnum getTypeCategory() const
		{
			return TypeCategoryEnum((this->flagBits >> kTypeCategoryShift) & kTypeCategoryMask);
		}

		BasicTypesEnum getTypeCode() const
		{
			return BasicTypesEnum((this->flagBits >> kTypeCodeShift) & kTypeCodeMask);
		}

		MemberTypeEnum getMemberType() const
		{
			return MemberTypeEnum(this->flagBits & kMemberTypeMask);
		}

		bool isAlwaysPersistField() const
		{
			return (this->flagBits & kAlwaysPersist) == kAlwaysPersist;
		}

		bool isBlittable() const
		{
			return (this->flagBits & kBlittable) == kBlittable;
		}

		bool isExposed() const
		{
			return (this->flagBits & kExposed) == kExposed;
		}

		bool isHomogeneous() const
		{
			return (this->flagBits & kHomogeneous) == kHomogeneous;
		}

		bool isLayoutImmutable() const
		{
			return (this->flagBits & kLayoutImmutable) == kLayoutImmutable;
		}

		bool isMetaField() const
		{
			return (this->flagBits & kMetadata) == kMetadata;
		}
	};



	class MemberInfo : Noncopyable
	{
	public:
		struct __declspec(align(8)) MemberInfoData
		{
			const char *name;
			MemberInfoFlags flags;
		};
		const void *m_infoData; //was void *

		const void *getInfoData() const
		{
			return this->m_infoData;
		}
		MemberInfo::MemberInfoData *getMemberInfoData() const
		{
			return (MemberInfo::MemberInfoData *)this->m_infoData;
		}

		const char *getName() const
		{
			return *(const char **)this->m_infoData;
		}

	};

	class TypeInfo : public MemberInfo
	{
	public:
		struct TypeInfoData
		{
			const char *name;
			MemberInfoFlags flags;
			unsigned __int16 totalSize;
			ModuleInfo *module;
			__int16 alignment;
			__int16 fieldCount;
			char pad1;
			char pad2;
		};
		TypeInfo *m_pNext;
		unsigned __int16 m_runtimeId;
		unsigned __int16 m_flags;


		TypeInfo::TypeInfoData * getTypeInfoData() const
		{
			return (TypeInfo::TypeInfoData *)m_infoData;
		}

		BasicTypesEnum getTypeCode() const
		{
			return BasicTypesEnum((getTypeInfoData()->flags.flagBits >> MemberInfoFlags::kTypeCodeShift) & MemberInfoFlags::kTypeCodeMask);
		}

		TypeCategoryEnum getTypeCategory() const
		{
			return TypeCategoryEnum((getTypeInfoData()->flags.flagBits >> MemberInfoFlags::kTypeCategoryShift) & MemberInfoFlags::kTypeCategoryMask);
		}

		MemberTypeEnum getMemberType() const
		{
			return MemberTypeEnum(getTypeInfoData()->flags.flagBits & MemberInfoFlags::kMemberTypeMask);
		}
	};

	struct PrimitiveTypeInfoVtbl
	{
		/*
		void(__thiscall *write)(fb::PrimitiveTypeInfo *this, fb::IObjectOutputStream *, const void *, unsigned int);
		void(__thiscall *read)(fb::PrimitiveTypeInfo *this, fb::IObjectInputStream *, void *, unsigned int);
		void(__thiscall *toString)(fb::PrimitiveTypeInfo *this, fb::StringBuilderBase *, const void *, fb::StringConvertParams *);
		fb::SkeletonNode *(__thiscall *toSkel)(fb::PrimitiveTypeInfo *this, const void *, fb::StringConvertParams *, fb::SkelConversionContext *);
		bool(__thiscall *parseSkel)(fb::PrimitiveTypeInfo *this, void *, fb::SkeletonNode *, fb::StringConvertParams *, fb::SkelConversionContext *);
		bool(__thiscall *parse)(fb::PrimitiveTypeInfo *this, void *, const char *, fb::StringConvertParams *);
		void(__thiscall *copy)(fb::PrimitiveTypeInfo *this, void *, const void *, fb::CopyContext *);
		int(__thiscall *doCompare)(fb::PrimitiveTypeInfo *this, const void *, const void *, fb::TypeInfo::CompareParams *, fb::TypeInfo::CompareState *);
		*/
	};

	class PrimitiveTypeInfo : public TypeInfo
	{
	public:
		struct PrimitiveTypeInfoData
		{
			TypeInfo::TypeInfoData tid;
		};


		PrimitiveTypeInfoVtbl *vfptr;

		PrimitiveTypeInfo::PrimitiveTypeInfoData * getPrimitiveTypeInfoData() const
		{
			return (PrimitiveTypeInfo::PrimitiveTypeInfoData *)this->m_infoData;
		}
	};


	struct FieldInfoVtbl
	{
		TypeInfo *(__thiscall *getDeclaringType)(FieldInfo *);
		unsigned int(__thiscall *getFieldIndex)(FieldInfo *);
	};

	class FieldInfo : public MemberInfo
	{
	public:
		const struct FieldInfoData
		{
			const char *name;
			MemberInfoFlags flags;
			unsigned __int16 fieldOffset;
			TypeInfo *fieldTypePtr;
		};
		FieldInfoVtbl *vfptr;

		FieldInfo::FieldInfoData * getFieldInfoInfoData() const
		{
			return (FieldInfo::FieldInfoData *)this->m_infoData;
		}

	};

	const class FieldInfoImpl : public FieldInfo
	{
	public:
		TypeInfo *m_declaringType;
		unsigned __int16 m_fieldIndex;
		unsigned __int16 m_attributeMask;
	};

	class ValueTypeFieldInfo : public FieldInfo 
	{
	};

	class ValueTypeInfo : public TypeInfo
	{
	public:
		struct ValueTypeInfoData
		{
			TypeInfo::TypeInfoData tid;
			const struct ValueTypeDefaultValue *defaultValue;
			const FieldInfo::FieldInfoData *fieldInfos;
		};

		FieldInfoImpl *const *m_fieldInfos;

		ValueTypeInfo::ValueTypeInfoData * getValueTypeInfoData() const
		{
			return (ValueTypeInfo::ValueTypeInfoData *)this->m_infoData;
		}


	};


	struct ArrayTypeInfoVtbl
	{
		unsigned int(__thiscall *getElementCount)(ArrayTypeInfo *, const void * data);
		void(__thiscall *setElementCount)(ArrayTypeInfo *, void * data, unsigned int newCount);
		void *(__thiscall *getRawDataPtr)(ArrayTypeInfo *, const void * data);
	};



	class ArrayTypeInfo : public TypeInfo
	{
	public:
		struct ArrayTypeInfoData
		{
			TypeInfo::TypeInfoData tid;
			TypeInfo *elementType;
		};
		ArrayTypeInfoVtbl *vfptr;

		ArrayTypeInfo::ArrayTypeInfoData * getArrayTypeInfoData() const
		{
			return (ArrayTypeInfo::ArrayTypeInfoData *)this->m_infoData;
		}
	};



	class ClassInfo : public TypeInfo
	{
	public:
		struct TypeInfoData
		{
			TypeInfo::TypeInfoData basicInfo;
			ClassInfo *superClass;
			ITypedObject *(__cdecl *createFun)(void *, MemoryArena *, bool);
			FieldInfo::FieldInfoData *fields;
		};

		ClassInfo *m_super;
		class DataContainer *m_defaultInstance;
		unsigned __int16 m_classId;
		unsigned __int16 m_lastSubClassId;
		ClassInfo *m_firstDerivedClass;
		ClassInfo *m_nextSiblingClass;
		FieldInfoImpl *const *m_fieldInfos;
		unsigned int m_totalFieldCount;


		ClassInfo::TypeInfoData * getClassTypeInfoData() const
		{
			return (ClassInfo::TypeInfoData *)this->m_infoData;
		}

		bool isSubclassOf(const ClassInfo *classPtr) const
		{
			return this->m_classId - static_cast<unsigned int>(classPtr->m_classId) <= classPtr->m_lastSubClassId - static_cast<unsigned int>(classPtr->m_classId);
		}

		bool isExactclassOf(const ClassInfo *classPtr) const
		{

			return this->m_classId == static_cast<unsigned int>(classPtr->m_classId);

		}
	};


	class DataContainerClassInfo : public ClassInfo 
	{
	private:
		std::uintptr_t m_opaqueTypeId; //vtable for valueinfo

	};


	class EntityDestroyer
	{
		__int64 vsptr;
	};

	enum Realm //has EnumTypeInfo
	{
		Realm_Client,
		Realm_Server,
		Realm_ClientAndServer,
		Realm_None,
		Realm_Pipeline
	};

	class EntityCreator : public EntityDestroyer
	{
	public:

		enum SpecialType
		{
			SpecialType_NotSpecial, SpecialType_ReferenceObjectData,
			SpecialType_SpatialEntity, SpecialType_Component
		};



		EntityCreator *m_previousCreator;
		EntityCreator *m_nextCreator;
		const ClassInfo *m_dataType;
		const int m_priority;
		const Realm m_realm;
		EntityCreator::SpecialType m_specialType;
		bool m_linked;
		bool m_isActive;
		bool m_isInitialized;


		static EntityCreator * * s_firstCreator; //todo get instance


		EntityCreator * * getFirstCreator(Realm realm)
		{
			s_firstCreator = (fb::EntityCreator **)0x1437F1050;
			return (&EntityCreator::s_firstCreator)[realm];
		}

		EntityCreator * getNextCreator()
		{
			return this->m_nextCreator;
		}

	};


	class EntityClassInfo : public ClassInfo
	{
	public:
		EntityCreator* destroyer[Realm::Realm_ClientAndServer]; //0x0050
		//EntityIterableLink firstIterableLink[Realm::Realm_ClientAndServer]; //0x0060
		//EntityIterableLink firstTrackedLink[Realm::Realm_ClientAndServer]; //0x00A0
	};

	class EnumTypeInfo : public TypeInfo
	{
	public:
		struct EnumTypeInfoData
		{
			TypeInfo::TypeInfoData tid;
			FieldInfo::FieldInfoData *fieldInfos;
		};

		FieldInfo *const *m_fieldInfos;


		EnumTypeInfo::EnumTypeInfoData * getEnumTypeInfoData() const
		{
			return (EnumTypeInfo::EnumTypeInfoData *)this->m_infoData;
		}

	};


	class HashString
	{
	public:
		uint32_t operator ()(const char* pString) const
		{
			uint32_t Hash = 5381;

			while (*pString)
				Hash = ((Hash << 5) + Hash) ^ *pString++; /* hash * 33 ^ c */

			return Hash;
		}
	};


}