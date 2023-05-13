#pragma once

#include "Utils.h"

#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>

#include <cstdlib>

namespace Tiny3D
{
	typedef void* (*AllocateFunction) (Uint64);
	typedef void (*FreeFunction) (void*);

	enum class AllocatorType : Uint64
	{
		StandardAllocator,
		LinearAllocator,
		StackAllocator,
		BlockAllocator,
	};

	struct Allocator
	{
		AllocatorType Type;
		void* RelatedData;

		AllocateFunction AllocateMemory;
		FreeFunction FreeMemory;
	};

	Allocator CreateStandardAllocator();

	// Realize this shit using SIMD
	void CopyMemory(void* destination, const void* source, Uint64 size);
	void SetMemory(void* data, Uint64 size, Uint8 value);
	void ZeroMemory(void* data, Uint64 size);
}
