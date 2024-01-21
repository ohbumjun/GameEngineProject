#include "hzpch.h"
#include "Hazel/Core/Allocation/Allocator/DefaultHeapAllocator.h"
#include <malloc.h>

namespace Hazel
{
	void* DefaultHeapAllocator::Allocate(size_t size)
	{
		return malloc(size);
	}

	void* DefaultHeapAllocator::Reallocate(void* ptr, size_t size)
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

	void DefaultHeapAllocator::Free(void* ptr)
	{
		free(ptr);
	}
}
