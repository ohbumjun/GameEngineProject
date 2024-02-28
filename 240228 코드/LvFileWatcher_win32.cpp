
#include <stdlib.h> // malloc
#include <stdio.h> // remove

#include <windows.h>
#include <shlwapi.h>

#include "editor/LvEditorContext.h"
#include "editor/LvEditorSettings.h"
#include "system/allocator/LvDefaultAllocator.h"
#include "system/LvString.h"
#include "system/LvStack.h"
#include "system/LvLog.h"

#include "editor/experiment/LvFileWatcher.h"
#include "editor/experiment/LvFileWatcherEventType.h"
#include "editor/experiment/LvFileWatcherDefine_win32.h"

#include "system/LvFileSystem.h"

#pragma comment(lib, "shlwapi.lib")     // Link to this file.

#ifdef _DEBUG
//#define __PRINT_FILE_WATCHER_NAVTIVE__
#endif

#ifdef __PRINT_FILE_WATCHER_NAVTIVE__
#define PRINT_FILE_WATCHER_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__);
#else
#define PRINT_FILE_WATCHER_LOG(fmt, ...)
#endif

using namespace Lv;
LV_NS_EDITOR_BEGIN

static LvHeapAllocator allocator;

static void lv_file_watcher_begin_read(LvFileWatcher* watcher)
{
	//memset(&watcher->overlapped, 0, sizeof(watcher->overlapped));

	BOOL success = ::ReadDirectoryChangesExW(
		watcher->directory,
		watcher->readBuffer,
		sizeof(watcher->readBuffer),
		watcher->recursive,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |// FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |// FILE_NOTIFY_CHANGE_LAST_ACCESS |
		FILE_NOTIFY_CHANGE_CREATION,// | FILE_NOTIFY_CHANGE_SECURITY,
		NULL,
		&watcher->overlapped,
		NULL,
		READ_DIRECTORY_NOTIFY_INFORMATION_CLASS::ReadDirectoryNotifyExtendedInformation);

	if (!success)
		LV_LOG(crash, "ReadDirectoryChangesW fail");
}

static char* lv_build_full_path(LvFileWatcher* watcher, FILE_NOTIFY_EXTENDED_INFORMATION* ev)
{
	LvWString fileName(ev->FileName, ev->FileNameLength / 2);

	char ns[LV_CHAR_INIT_LENGTH] = { 0, };
	lv_path_system_to_utf8(ns, fileName.c_str());
	LvString normalizedFileName(ns);
	
	size_t totalLen = watcher->watchDir.Length() + 1 + normalizedFileName.Length() + 1;
	char* ret = (char*)allocator.Alloc(totalLen * sizeof(char), __FILE__, __LINE__);
	memcpy(ret, watcher->watchDir.c_str(), watcher->watchDir.Length());
	ret[watcher->watchDir.Length()] = LV_DIRECTORY_SEPARATOR_CHAR;
	memcpy(ret + watcher->watchDir.Length() + 1, normalizedFileName.c_str(), normalizedFileName.Length());
	ret[totalLen - 1] = '\0';

	return ret;
}

LvFileWatcher* lv_file_watcher_create(LvFileWatcherCreateFlags flags, LvFileWatcherEventType types, const char* path)
{
	LvFileWatcher* w = (LvFileWatcher*)allocator.Alloc(sizeof(LvFileWatcher), __FILE__, __LINE__);
	w = new (w) LvFileWatcher();

	w->watchDir = path;
	w->recursive = (flags & LV_FILEWATCHER_RECURSIVE) > 0;
	w->blocking = (flags & LV_FILEWATCHER_BLOCKING) > 0;
	w->pause = false;

	wchar_t wc[LV_CHAR_INIT_LENGTH];
	lv_path_utf8_to_system((void*) wc, path, true);

	w->directory = ::CreateFile(wc,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, // security descriptor
		OPEN_EXISTING, // how to create
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, // file attributes details in https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
		NULL); // file with attributes to copy

	// handle error ...
	if (w->directory == INVALID_HANDLE_VALUE)
		return nullptr;

	w->overlapped.hEvent = CreateEvent(NULL, true, false, NULL);
	w->checkEvent = CreateEvent(NULL, false, false, NULL);
	w->eventList.Add(w->overlapped.hEvent);
	w->eventList.Add(w->checkEvent);

	lv_file_watcher_begin_read(w);
	return w;
}

void lv_file_watcher_destroy(LvFileWatcher* watcher)
{
	::CloseHandle(watcher->directory);
	::CloseHandle(watcher->overlapped.hEvent);
	::CloseHandle(watcher->checkEvent);

	watcher->eventList.Clear();
	watcher->~LvFileWatcher();
	allocator.Free(watcher);
}

void lv_file_watcher_get_event(LvFileWatcher* watcher, LvList<LvFileWatcherEventSet>* watchDatas)
{
	DWORD bytes;

	//통신 디바이스에서 겹치는 작업의 결과를 검색
	BOOL res = ::GetOverlappedResult(watcher->directory,
		&watcher->overlapped,
		&bytes,
		watcher->blocking);

	if (!res)
	{
		lv_file_watcher_begin_read(watcher);
		return;
	}

	FILE_NOTIFY_EXTENDED_INFORMATION* notifyInfo = reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(watcher->readBuffer);
	bool isLoop = false;

	do {
		
		char* path = lv_build_full_path(watcher, notifyInfo);
		isLoop = notifyInfo->NextEntryOffset != 0;

		LvFileWatcherEventSet data;
		FILE_NOTIFY_EXTENDED_INFORMATION* currentNotiInfo = notifyInfo;

		notifyInfo = reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>((reinterpret_cast<char*>(notifyInfo) + notifyInfo->NextEntryOffset));

		data.path = path;
		data.action = currentNotiInfo->Action;
		data.lastAccessTime = currentNotiInfo->LastAccessTime;

		
		if (!lv_editor_get_context()->settings.disableWatcherEventLog)
		{
			const int pathHashCode = hashCode(path);
			if (!(watcher->lastPathHashCode == pathHashCode
				&& watcher->lastAccessTime.LowPart == data.lastAccessTime.LowPart
				&& watcher->lastAccessTime.HighPart == data.lastAccessTime.HighPart))
			{
				watcher->lastPathHashCode = pathHashCode;
				watcher->lastAccessTime = data.lastAccessTime;
				FILETIME ft;
				SYSTEMTIME st;
				ft.dwLowDateTime = data.lastAccessTime.LowPart;
				ft.dwHighDateTime = data.lastAccessTime.HighPart;
				if (FileTimeToSystemTime(&ft, &st))
				{
					static const char* s_actionTypes[]
						{
							"",
							"FILE_ACTION_ADDED",
							"FILE_ACTION_REMOVED",
							"FILE_ACTION_MODIFIED",
							"FILE_ACTION_RENAMED_OLD_NAME",
							"FILE_ACTION_RENAMED_NEW_NAME"
						};

					// https://learn.microsoft.com/ko-kr/windows/win32/api/winnt/ns-winnt-file_notify_extended_information
					LV_CHECK(data.action <= 5, "Unknown action type");

					LV_SYSTEM_LOG(debug, "Watcher Event:: Path : [%s] Action : [%s] LastAccessTime : [%04d-%02d-%02d UTC%02d:%02d:%02d.%03d]", data.path.c_str(), s_actionTypes[data.action],
						st.wYear, st.wMonth, st.wDay,
						st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
				}
				else
				{
					LV_SYSTEM_LOG(debug, "Watcher Event:: Path : [%s] Action : [%d]", data.path.c_str(), data.action);
				}
			}
		}

		if (currentNotiInfo->FileSize.QuadPart == 0 && (currentNotiInfo->FileAttributes != FILE_ATTRIBUTE_DIRECTORY) && (currentNotiInfo->Action == FILE_ACTION_MODIFIED))
		{
			continue;
		}

		if ((currentNotiInfo->FileAttributes != FILE_ATTRIBUTE_DIRECTORY) && (currentNotiInfo->Action == FILE_ACTION_MODIFIED) && !(currentNotiInfo->LastModificationTime.QuadPart == currentNotiInfo->LastChangeTime.QuadPart))// && currentNotiInfo->LastChangeTime.QuadPart == currentNotiInfo->LastAccessTime.QuadPart))
		{
			continue;
		}

		//When Create By FileDialog. Create->Remove->Create->Modify (??) 
		/*if (currentNotiInfo->Action == FILE_ACTION_REMOVED
			&& currentNotiInfo->CreationTime.QuadPart == currentNotiInfo->LastAccessTime.QuadPart
			&& currentNotiInfo->FileSize.QuadPart == 0)
		{
			continue;
		}*/

		watchDatas->Add(data);

	} while (isLoop);
	
	lv_file_watcher_begin_read(watcher);
	 
}


#undef PRINT_FILE_WATCHER_LOG
LV_NS_EDITOR_END
