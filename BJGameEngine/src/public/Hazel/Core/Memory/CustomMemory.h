#pragma once

#ifndef CUSTOM_MEMORY_H
#define CUSTOM_MEMORY_H

#include <malloc.h>

/*
>> size + alignment - 1
- size + alignment - 1  : ���� bitwise AND ������ �� �� ��Ȯ�� ����� ��� �ȴ�.
						 size �� alignment boundary �� ��� ������ ���̰� ���ش�.

- alignment - 1			: ��� bit �� 1 �� �Ѵ�. ex) alignment === 4 -> 0b11

- ~(aligment - 1)		: ~ �� ��� bit �� �����´�.��, ��� bit �� 0���� �ٽ� �ǵ����� ���

- &		: size + alignement - 1 ��Ʈ�� �߿��� '1' ���� ���ܵδ� ȿ�� 

< ���� >
  // Example 1: Aligning a size to a 4-byte boundary
    int size1 = 10; // Input size
    int alignment1 = 4; // Alignment boundary

    int alignedSize1 = lv_align_up(size1, alignment1);

    printf("Original Size: %d\n", size1);
    printf("Aligned Size (4-byte boundary): %d\n", alignedSize1);

    // Example 2: Aligning a size to an 8-byte boundary
    int size2 = 17; // Input size
    int alignment2 = 8; // Alignment boundary

    int alignedSize2 = lv_align_up(size2, alignment2);

    printf("Original Size: %d\n", size2);
    printf("Aligned Size (8-byte boundary): %d\n", alignedSize2);

    Original Size: 10
    Aligned Size (4-byte boundary): 12

    ex) 13 : 1101
         3 : 0011
        ~3 : 1100

        -> 12

    Original Size: 17
    Aligned Size (8-byte boundary): 24
*/
#define pr_align_up(size, alignment) ((size + alignment - 1) & ~(alignment - 1))

namespace Hazel
{
    class MemoryUtil
    {
    public:
        static void *HazelMalloc(size_t size);
        static void *HazelRealloc(void *ptr, size_t size);
        static void *HazelCalloc(size_t number, size_t size);
        static void HazelFree(void *ptr);
        static void *HazelAlignedAlloc(size_t size, size_t alignment);
        static void *HazelAlignedRealloc(void *ptr, size_t alignment, size_t new_size);
        static void HazelAlignedFree(void *pointer);

    };
}


#endif // MY_HEADER_H