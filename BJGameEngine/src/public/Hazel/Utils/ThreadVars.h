#pragma once

#include "hzpch.h"

#define THREAD_WAIT_INFINITE (~(uint32)0)
#define THREAD_WAIT_TIMEDOUT 1
namespace Hazel
{

typedef struct
{
    char *name;
    unsigned long id;
    HANDLE handle;
    int stackSize;
    int affinity;

} ThreadInfo;
typedef enum
{
    RUNNING = 0,
    BACKGROUND = 4,
    UNSTARTED = 8,
    STOPPED = 16,
    WAIT_SLEEP_JOIN = 32,
    SUSPENDED = 64,
    ABORTED = 256,
    WAIT = 512

} ThreadState;

typedef enum
{
    LOW = -2,
    UNKNOWN = 0,
    HIGH = 2,

} ThreadPriority;

typedef struct
{
    CONDITION_VARIABLE handle;

} ConditionVariable;

typedef struct
{
    // ex. �ʱⰪ == 5
    // ���� ��� �����尡 signal ��������.
    // ��, ���� ���� (signale �� ������ --, relaese �� ������ ++)
    int count;
    HANDLE handle;

} Semaphore;

struct CRIC_SECT
{
    CRITICAL_SECTION handle;
    // ��Ƽ������ ȯ�濡�� ,���� �����忡 ���� ���� �� �����Ƿ�
    // ����ȭ x
    volatile bool isInit;
};

struct Atomic
{
    Atomic() = default;

    Atomic(const Atomic &o);

    int GetVal();

    // volatile int atomicVal;
    std::atomic<int> atomicVal;
};


class SpinLock
{
    friend class ThreadUtils;

public:
    SpinLock();
    void Init();
    bool TryLock();
    void Lock();
    void Unlock();

private:
    Atomic flag;
};
