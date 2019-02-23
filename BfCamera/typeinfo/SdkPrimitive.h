#pragma once



#include <string>
#include <vector>


#include "Sdk.h"


namespace Backend
{
	struct SDK_PRIMITIVE
	{
		std::string m_name;
		unsigned int m_totalSize;
		unsigned int m_alignment;
		unsigned int m_fieldCount;

		fb::BasicTypesEnum m_basictypecode;

	};
	typedef SDK_PRIMITIVE* PSDK_PRIMITIVE;
}