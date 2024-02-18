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

		/*_aligned_realloc �� ���⼭ alignment �� �����ִ� ���*/
		return realloc(ptr, size);

		/*
		realloc �̳� _aligned_realloc ��� resize �� �޸� ����� ���ο� �����͸� ������ �� �ִٴ� ���̴�.
		��, ���� ��ġ���� resize �� �� ���ٸ�, ���ο� block �� �Ҵ��ϴ� ����̴�.
		�׸��� ���� block �� ������, ���ο� block ���� �������ִ� ����̴�.

		�׷��Ƿ�, �ش� �Լ��� ������� original pointer �� �ٽ� �����Ͽ�
		dangling �� ���� �ʰ� �ؾ� �Ѵ�.
		*/
	}

	void DefaultHeapAllocator::Free(void* ptr)
	{
		free(ptr);
	}
}
