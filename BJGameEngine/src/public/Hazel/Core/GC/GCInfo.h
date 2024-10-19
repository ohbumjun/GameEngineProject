#pragma once

struct GCAllocationHeader
{
    size_t blockSize;
    char padding; // 1byte -> 8 bit -> 256 까지 표현가능
    bool isRoot = false;
    bool visited = false; // markBit 에 해당
    int refCnt = 0;
};