#pragma once

struct GCAllocationHeader
{
    size_t blockSize;
    char padding;    // 1byte -> 8 bit -> 256 ���� ǥ������
    bool isRoot = false;
    bool visited = false; // markBit �� �ش�
    int   refCnt = 0;
};