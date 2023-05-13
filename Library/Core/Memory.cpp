#include "Memory.h"

namespace Tiny3D
{
	Allocator CreateStandardAllocator() {
		auto allocate = [](Uint64 size)
		{
			return malloc(size);
		};

		auto deallocate = [](void* ptr)
		{
			free(ptr);
		};

		return { AllocatorType::StandardAllocator, nullptr, allocate, deallocate };
	}

    void CopyMemory(void* destination, const void* source, Uint64 size)
    {
        if (destination == nullptr || source == nullptr)
        {
            // TODO: debug log here
            return;
        }

        if (destination == source)
        {
            // TODO: debug log here
            return;
        }

        Uint64 i = 0;
        for (; i + 32 <= size; i += 32)
        {
            Uint8* iSourcePtr = (Uint8*) source + i;
            Uint8* iDestinationPtr = (Uint8*) destination + i;

            __m256i iSourceSequence = _mm256_lddqu_si256((__m256i const*) iSourcePtr);
            _mm256_storeu_si256((__m256i*) iDestinationPtr, iSourceSequence);
        }

        for (; i + 16 <= size; i += 16)
        {
            // SSE stuff here (copying by 128 bits)
            Uint8* iSourcePtr = (Uint8*) source + i;
            Uint8* iDestinationPtr = (Uint8*) destination + i;

            __m128i iSourceSequence = _mm_lddqu_si128((__m128i const*) iSourcePtr);
            _mm_storeu_si128((__m128i*) iDestinationPtr, iSourceSequence);
        }

        for (; i < size; i += 1)
        {
            Uint8* iSourcePtr = (Uint8*) source + i;
            Uint8* iDestinationPtr = (Uint8*) destination + i;

            *iDestinationPtr = *iSourcePtr;
        }
    }

    void SetMemory(void* data, Uint64 size, Uint8 value)
    {
        if (data == nullptr)
            return;

        Uint64 i = 0;
        for (; i + 32 <= size; i += 32) {
            Uint8* iDestinationPtr = (Uint8*) data + i;

            __m256i iSourceSequence = _mm256_set1_epi8(static_cast<Int8>(value));
            _mm256_storeu_si256((__m256i*) iDestinationPtr, iSourceSequence);
        }

        for (; i + 16 <= size; i += 16) {
            Uint8* iDestinationPtr = (Uint8*) data + i;

            __m128i iSourceSequence = _mm_set1_epi8(static_cast<Int8>(value));
            _mm_storeu_si128((__m128i*) iDestinationPtr, iSourceSequence);
        }

        for (; i < size; i += 1)
        {
            Uint8* iDestinationPtr = (Uint8*) data + i;
            *iDestinationPtr = value;
        }
    }

    void ZeroMemory(void* data, Uint64 size)
    {
        SetMemory(data, size, 0);
    }
}
