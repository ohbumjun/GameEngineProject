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
    // _aligned_malloc �� malloc �Լ��� �ֿ��� ��������
    // alignment �� �ִ�.
    /*
	size_t size = 16;  // Size of memory to allocate
    size_t alignment = 32;  // Alignment value in bytes

    // Allocate memory with specified size and alignment
    void* memory = _aligned_malloc(size, alignment);

	�� ��ʿ� ���� _aligned_malloc �� Ư�� ũ���� �޸� ���� Ư�� alignment �� �°�
	�Ҵ��ϴ� ���̴�.

	alignemnt ����, �Ҵ�� �޸� ����� ���� �ּ� ��ġ�� �����Ѵ�. 

	�ݸ�, malloc �� Ư�� ũ���� �޸� ���� �Ҵ��ϴ� �͸� �� ��
	Ư�� alignemt �� ���������� �ʴ´�
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