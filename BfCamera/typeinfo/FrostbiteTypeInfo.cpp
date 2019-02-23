#include "FrostbiteTypeInfo.h"


namespace fb
{
	const std::string toCTypeString(BasicTypesEnum typeCode)
	{
		std::string result;

		switch (typeCode)
		{
		case kTypeCode_Void:
			result = "None";
			break;
		case kTypeCode_DbObject:
			result = "None";
			break;
		case kTypeCode_ValueType:
			result = "None";
			break;
		case kTypeCode_Class:
			result = "None";
			break;
		case kTypeCode_Array:
			result = "None";
			break;
		case kTypeCode_FixedArray:
			result = "None";
			break;
		case kTypeCode_String:
			result = "None";
			break;
		case kTypeCode_CString:
			result = "char *";
			break;
		case kTypeCode_Enum:
			result = "None";
			break;
		case kTypeCode_FileRef:
			result = "None";
			break;
		case kTypeCode_Boolean:
			result = "bool";
			break;
		case kTypeCode_Int8:
			result = "__int8";
			break;
		case kTypeCode_Uint8:
			result = "unsigned __int8";
			break;
		case kTypeCode_Int16:
			result = "__int16";
			break;
		case kTypeCode_Uint16:
			result = "unsigned __int16";
			break;
		case kTypeCode_Int32:
			result = "__int32";
			break;
		case kTypeCode_Uint32:
			result = "unsigned __int32";
			break;
		case kTypeCode_Int64:
			result = "__int64";
			break;
		case kTypeCode_Uint64:
			result = "unsigned __int64";
			break;
		case kTypeCode_Float32:
			result = "float";
			break;
		case kTypeCode_Float64:
			result = "double";
			break;
		case kTypeCode_Guid:
			result = "None";
			break;
		case kTypeCode_SHA1:
			result = "None";
			break;
		case kTypeCode_ResourceRef:
			result = "None";
			break;
		default:
			result = "None";
			break;
		}
		return result;
	}


	const char * toString(BasicTypesEnum typeCode)
	{
		const char *result;

		switch (typeCode)
		{
		case kTypeCode_Void:
			result = "Void";
			break;
		case kTypeCode_DbObject:
			result = "DbObject";
			break;
		case kTypeCode_ValueType:
			result = "ValueType";
			break;
		case kTypeCode_Class:
			result = "Class";
			break;
		case kTypeCode_Array:
			result = "Array";
			break;
		case kTypeCode_FixedArray:
			result = "FixedArray";
			break;
		case kTypeCode_String:
			result = "String";
			break;
		case kTypeCode_CString:
			result = "CString";
			break;
		case kTypeCode_Enum:
			result = "Enum";
			break;
		case kTypeCode_FileRef:
			result = "FileRef";
			break;
		case kTypeCode_Boolean:
			result = "Boolean";
			break;
		case kTypeCode_Int8:
			result = "Int8";
			break;
		case kTypeCode_Uint8:
			result = "Uint8";
			break;
		case kTypeCode_Int16:
			result = "Int16";
			break;
		case kTypeCode_Uint16:
			result = "Uint16";
			break;
		case kTypeCode_Int32:
			result = "Int32";
			break;
		case kTypeCode_Uint32:
			result = "Uint32";
			break;
		case kTypeCode_Int64:
			result = "Int64";
			break;
		case kTypeCode_Uint64:
			result = "Uint64";
			break;
		case kTypeCode_Float32:
			result = "Float32";
			break;
		case kTypeCode_Float64:
			result = "Float64";
			break;
		case kTypeCode_Guid:
			result = "Guid";
			break;
		case kTypeCode_SHA1:
			result = "SHA1";
			break;
		case kTypeCode_ResourceRef:
			result = "ResourceRef";
			break;
		default:
			result = "Unknown";
			break;
		}
		return result;
	}

	const char * toString(MemberTypeEnum memberType)
	{
		const char *result;

		switch (memberType)
		{
		case kMemberType_Field:
			result = "Field";
			break;
		case kMemberType_TypeInfo:
			result = "TypeInfo";
			break;
		default:
			result = "Unknown";
			break;
		}
		return result;
	}


	const char * toString(TypeCategoryEnum typeCategory)
	{
		const char *result;

		switch (typeCategory)
		{
		case kTypeCategory_NotApplicable:
			result = "NotApplicable";
			break;
		case kTypeCategory_Class:
			result = "Class";
			break;
		case kTypeCategory_ValueType:
			result = "ValueType";
			break;
		case kTypeCategory_PrimitiveType:
			result = "PrimitiveType";
			break;
		default:
			result = "Unknown";
			break;
		}
		return result;
	}

}