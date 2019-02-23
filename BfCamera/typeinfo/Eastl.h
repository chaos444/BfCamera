#ifndef __EASTL_H__
#define __EASTL_H__

/*
#include "EASTL/algorithm.h"
#include "EASTL/allocator.h"
#include "EASTL/bitset.h"
#include "EASTL/core_allocator_adapter.h"
#include "EASTL/deque.h"
#include "EASTL/fixed_allocator.h"
#include "EASTL/fixed_hash_map.h"
#include "EASTL/fixed_hash_set.h"
#include "EASTL/fixed_list.h"
#include "EASTL/fixed_map.h"
#include "EASTL/fixed_set.h"
#include "EASTL/fixed_string.h"
#include "EASTL/fixed_substring.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/functional.h"
#include "EASTL/hash_map.h"
#include "EASTL/hash_set.h"
#include "EASTL/heap.h"
#include "EASTL/iterator.h"
#include "EASTL/list.h"
#include "EASTL/map.h"
#include "EASTL/memory.h"
#include "EASTL/set.h"
#include "EASTL/sort.h"
#include "EASTL/string.h"
#include "EASTL/type_traits.h"
#include "EASTL/utility.h"
#include "EASTL/vector.h"
*/


namespace eastl
{
	class allocator
	{
	public:
		__forceinline allocator()
		{
		}

		__forceinline allocator(eastl::allocator *__formal)
		{
		}

		__forceinline void set_name(const char *__formal)
		{
		}

		__forceinline const char* get_name()
		{
			return "EASTL";
		}

		__forceinline void* allocate(unsigned int n, int flags)
		{
			return malloc(n);
		}

		__forceinline void deallocate(void *p, unsigned int __formal)
		{
			free(p);
		}
	};

	template<typename T> class BasicArray
	{
	public:
		T* mpBegin;
	};

	template<typename T> class Array : eastl::BasicArray<T>
	{

	};

}



#endif