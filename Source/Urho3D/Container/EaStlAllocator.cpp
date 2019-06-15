#include "EaStlAllocator.h"

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>
#include <new>
#include <stdio.h>

#if !defined(EA_PLATFORM_MICROSOFT) || defined(EA_PLATFORM_MINGW)
#include <stdlib.h>
#endif

	namespace Internal
	{
		void* EASTLAlignedAlloc(size_t size, size_t alignment)
		{
#ifdef EA_PLATFORM_MICROSOFT
			return _aligned_malloc(size, alignment);
#else
			void *p = nullptr;
			alignment = alignment < sizeof(void *) ? sizeof(void *) : alignment;
			posix_memalign(&p, alignment, size);
			return p;
#endif
		}

		void EASTLAlignedFree(void* p)
		{
#ifdef EA_PLATFORM_MICROSOFT
			_aligned_free(p);
#else
			free(p);
#endif
		}
	}

void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return Internal::EASTLAlignedAlloc(size, 16);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return Internal::EASTLAlignedAlloc(size, alignment);
}