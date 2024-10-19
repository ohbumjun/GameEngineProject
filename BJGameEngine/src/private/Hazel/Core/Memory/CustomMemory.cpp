#pragma once

#include "Hazel/Core/Memory/CustomMemory.h"

namespace Hazel
{
void *MemoryUtil::HazelMalloc(size_t size)
{
    return malloc(size);
}

void *MemoryUtil::HazelRealloc(void *ptr, size_t size)
{
    // resize previous allocated block
    // 1) pointer to original memory block
    // 2) new size

    /*_aligned_realloc 은 여기서 alignment 만 맞춰주는 기능*/
    return realloc(ptr, size);

    /*
	realloc 이나 _aligned_realloc 모두 resize 된 메모리 블록의 새로운 포인터를 리턴할 수 있다는 것이다.
	즉, 기존 위치에서 resize 될 수 없다면, 새로운 block 을 할당하는 방식이다.
	그리고 기존 block 의 내용을, 새로운 block 으로 복사해주는 방식이다.

	그러므로, 해당 함수의 결과값을 original pointer 에 다시 세팅하여
	dangling 이 나지 않게 해야 한다.
	*/
}

void *MemoryUtil::HazelCalloc(size_t number, size_t size)
{
    return calloc(number, size);
}

void HazelFree(void *ptr)
{
    free(ptr);
}

void *MemoryUtil::HazelAlignedAlloc(size_t size, size_t alignment)
{
    // _aligned_malloc 와 malloc 함수의 주요한 차이점은
    // alignment 에 있다.
    /*
	size_t size = 16;  // Size of memory to allocate
    size_t alignment = 32;  // Alignment value in bytes

    // Allocate memory with specified size and alignment
    void* memory = _aligned_malloc(size, alignment);

	위 사례와 같이 _aligned_malloc 은 특정 크기의 메모리 블럭을 특정 alignment 에 맞게
	할당하는 것이다.

	alignemnt 값은, 할당된 메모리 블록의 시작 주소 위치를 결정한다. 

	반면, malloc 은 특정 크기의 메모리 블럭을 할당하는 것만 할 뿐
	특정 alignemt 를 맞춰주지는 않는다
	*/
    return _aligned_malloc(size, alignment);
}

void *MemoryUtil::HazelAlignedRealloc(void *ptr, size_t alignment, size_t new_size)
{
    return _aligned_realloc(ptr, new_size, alignment);
}

void MemoryUtil::HazelAlignedFree(void *pointer)
{
    _aligned_free(pointer);
}
} // namespace Hazel