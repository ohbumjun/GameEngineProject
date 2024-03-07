#include "editor/experiment/LvFileWatcherEventType.h"
#include "system/LvString.h"

#define watcherBufferMaxCount 2048

LV_NS_EDITOR_BEGIN

struct LvFileWatcher
{
    LvFileWatcher()
    {
    }

    BOOL recursive = false; //Watch SubTree
    BOOL blocking = false;  //is sync (is wait until recieve result..)
    BOOL pause = false;     //is pause

    LvString watchDir;

    HANDLE directory = nullptr;  //watch root directory
    HANDLE checkEvent = nullptr; //break lock event
    OVERLAPPED overlapped = {};

    LvList<HANDLE> eventList; //list[overlapped.event, checkEvent]

    int lastPathHashCode = 0;
    LARGE_INTEGER lastAccessTime;

    FILE_NOTIFY_EXTENDED_INFORMATION readBuffer[watcherBufferMaxCount];
};


struct LvFileWatcherEventSet
{
    LvString path = "";
    DWORD action = -1;
    LARGE_INTEGER lastAccessTime;

    bool operator==(const LvFileWatcherEventSet &data)
    {
        return (path == data.path) && (action == data.action);
    }
};
LV_NS_EDITOR_END
