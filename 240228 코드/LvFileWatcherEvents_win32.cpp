#include "LvPrecompiled.h"
#include "editor/experiment/LvFileWatcherEvents.h"

#include <windows.h>
#include <shlwapi.h>

#include "system/LvString.h"
#include "system/LvStack.h"
#include "system/LvHashtable.h"
#include "system/LvHashSet.h"
#include "system/LvThread.h"
#include "system/LvLog.h"
#include "system/LvFileSystem.h"
#include "system/LvThread_Extension.h"
#include "editor/private/project/LvFileManager.h"
#include "editor/LvEditor.h"
#include "editor/LvEditorContext.h"

#include "editor/experiment/LvFileWatcherEventType.h"
#include "editor/experiment/LvFileWatcherDefine_win32.h"
#include "engine/thread/LvDispatchQueue.h"

#pragma comment(lib, "shlwapi.lib")     // Link to this file.

#ifdef _DEBUG
//#define __PRINT_WATCH_EVENT_NAVTIVE__
#endif

#ifdef __PRINT_WATCH_EVENT_NAVTIVE__
#define PRINT_EVENT_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__);
#else
#define PRINT_EVENT_LOG(fmt, ...)
#endif

using namespace Lv;

bool	g_shutdown = false;

LV_NS_EDITOR_BEGIN
struct LvFileWatcherDataSet
{
	LvFileWatcherEventHandler* handler = nullptr;
	LvFileWatcher* watcher = nullptr;
};

static LvHashtable<LvThread*, LvFileWatcherDataSet> s_threads;
static LvHashtable<LvFileWatcher*, LvThread*> s_registFileWatcher;

void dir_watcher_listen(LPCSTR dir, LvFileWatcherEventHandler* handler, LvFileWatcherEventType eventType, const char* pastDir)
{
	WIN32_FIND_DATA ffd;
	wchar_t allDir[LV_CHAR_INIT_LENGTH];
	lv_path_utf8_to_system((void*)allDir, lv_path_combine(dir, "*").c_str());
	HANDLE hFind = FindFirstFile((LPCWSTR)allDir, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
			continue;

		char fileName[LV_CHAR_INIT_LENGTH];
		lv_path_system_to_utf8(fileName, (void*)ffd.cFileName);
		LvString path = lv_path_combine(dir, fileName);

		//dir
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			switch (eventType)
			{
			case LV_FILEWATCHER_EVENT_CREATE:
				dir_watcher_listen(path.c_str(), handler, eventType, nullptr);
				break;
			case LV_FILEWATCHER_EVENT_REMOVE:
				LV_DEBUG_BREAK(); //must not be called remove. remove는 모든 파일이 알아서 watcher에서 불린다.
				break;
			case LV_FILEWATCHER_EVENT_MOVE:
			{
				LvString newPath = lv_path_combine(pastDir, fileName);
				dir_watcher_listen(path.c_str(), handler, eventType, newPath.c_str());
				break;
			}
			}
		}

		//file
		else
		{
			switch (eventType)
			{
			case LV_FILEWATCHER_EVENT_CREATE:
				handler->callback(LV_FILEWATCHER_EVENT_CREATE, path.c_str(), nullptr);
				PRINT_EVENT_LOG("-DIR-CALLBACK---CREATE----\t%s\n", path.c_str());
				break;
			case LV_FILEWATCHER_EVENT_REMOVE:
				LV_DEBUG_BREAK(); //must not be called remove. remove는 모든 파일이 알아서 watcher에서 불린다.
				break;
			case LV_FILEWATCHER_EVENT_MOVE:
			{
				handler->callback(LV_FILEWATCHER_EVENT_MOVE, lv_path_combine(pastDir, fileName).c_str(), path.c_str());
				PRINT_EVENT_LOG("-DIR-CALLBACK---MOVE------\t%s ->\n %s\n", lv_path_combine(pastDir, fileName).c_str(), path.c_str());
				break;
			}
			}
		}


	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	switch (eventType)
	{
	case LV_FILEWATCHER_EVENT_CREATE:
		handler->callback(LV_FILEWATCHER_EVENT_CREATE, dir, nullptr);
		PRINT_EVENT_LOG("-DIR-CALLBACK---CREATE----DIR:\t%s\n", dir);
		break;
	case LV_FILEWATCHER_EVENT_REMOVE:
		LV_DEBUG_BREAK(); //must not be called remove. remove는 모든 파일이 알아서 watcher에서 불린다.
		break;
	case LV_FILEWATCHER_EVENT_MOVE:
		handler->callback(LV_FILEWATCHER_EVENT_MOVE, pastDir, dir);
		PRINT_EVENT_LOG("-DIR-CALLBACK---MOVE------DIR:\t%s ->\n %s\n", pastDir, dir);
		break;
	}
}

void filteredCallback(LvFileWatcherEventHandler* handler, LvList<LvFileWatcherEventSet>& sets)
{
	static LvQueue<LvFileWatcherEventSet> dataQueue;

	for(auto& each : sets)
	{
		dataQueue.Enqueue(each);
	}

	if (!dataQueue.IsEmpty())
	{
		LvFileWatcherEventSet prevData;
		LvFileWatcherEventSet nextData;

		static LvFileWatcherEventSet prevDirData; //오직 잘라내기를 확인하기 위한 용도. 다른 용도로는 쓰이지 않는다.
		static LvStack<LvFileWatcherEventSet> directoriesStack;
		static LvHashSet<LvString> directoriesSet;

		while (!dataQueue.IsEmpty())
		{
			LvFileWatcherEventSet data = dataQueue.Dequeue();

			//중복된 이벤트 제거.
			if (!dataQueue.IsEmpty())
			{
				nextData = *dataQueue.begin();
				if (data == nextData) continue;
			}

			bool isDirectory = PathIsDirectoryA(data.path.c_str());
			bool isFile = !isDirectory;

			if (isFile &&
				directoriesStack.Count() > 0 &&
				data.path.IndexOf(directoriesStack.Peek().path) >= 0)
			{
				//만일 create로 들어온 폴더 안의 파일들이면 넘긴다.
				//이후 dir_listen에서 한 번에 처리해줄 것이다.
				PRINT_EVENT_LOG("---REALINPUT---SKIPPED:\t%s\n", set.src.c_str());
				continue;
			}

			switch (data.action)
			{

			case FILE_ACTION_ADDED:
			{
				if (!(lv_directory_exist(data.path.c_str()) || lv_file_exist(data.path.c_str())))
				{
					break;
				}

				if (isDirectory)
				{
					if (prevData.action == FILE_ACTION_REMOVED
						&& lv_path_name(prevData.path.c_str()) == lv_path_name(data.path.c_str()))
					{
						dir_watcher_listen(data.path.c_str(), handler, LV_FILEWATCHER_EVENT_MOVE, prevData.path.c_str());
					}
					else if (!directoriesSet.Contains(data.path) &&
						(directoriesStack.Count() == 0 || data.path.IndexOf(directoriesStack.Peek().path) < 0))
					{
						PRINT_EVENT_LOG("---STACK------CREATE:\t%s\n", set.src.c_str());
						directoriesStack.Push(data);
						directoriesSet.Add(data.path);
					}
				}
				else
				{
					const char* prevFileName = PathFindFileNameA(prevData.path.c_str());
					const char* fileName = PathFindFileNameA(data.path.c_str());

					if (strcmp(prevFileName, fileName) == 0 && prevData.action == FILE_ACTION_REMOVED)
					{
						//Move
						handler->callback(LV_FILEWATCHER_EVENT_MOVE, prevData.path.c_str(), data.path.c_str());
					}
					else
					{
						//Create
						handler->callback(LV_FILEWATCHER_EVENT_CREATE, data.path.c_str(), nullptr);
					}
				}
				break;
			}


			case FILE_ACTION_REMOVED:
			{
				const char* nextFileName = PathFindFileNameA(nextData.path.c_str());
				const char* fileName = PathFindFileNameA(data.path.c_str());

				if (strcmp(nextFileName, fileName) == 0 && nextData.action == FILE_ACTION_ADDED)
				{
					break;
				}
				else
				{
					//Remove
					handler->callback(LV_FILEWATCHER_EVENT_REMOVE, data.path.c_str(), nullptr);
				}

				break;
			}

			case FILE_ACTION_MODIFIED:
			{
				// @donghun 윈도우에서 파일을 여러개 제거하거나 생성할 때 이벤트가 간혹 들어오지 않는 경우가 있는데
				//          마지막엔 부모 경로의 MODIFY 이벤트가 발생한다.
				//
				//          예를 들어 parent의 자식들을 1~7까지 선택 후 지운다면 아래와 같은 이벤트가 발생한다.
				//          Event : [FILE_ACTION_REMOVED] parent / directory(1)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(2)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(3)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(4)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(5)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(6)
				//          Event : [FILE_ACTION_REMOVED] parent / directory(7)
				//          Event : [FILE_ACTION_MODIFIED] parent
				//
				//          따라서 마지막으로 들어오는 부모 경로의 자식들을 체크하여 들어오지 않은 이벤트를 체크해야한다.
				// if (isDirectory)
				// {
				// 	continue;
				// }

				//prev가 CREATE라면 이후 MODIFY는 무시
				if (prevData.path == data.path && prevData.action == FILE_ACTION_ADDED)
				{
					break;
				}
				else
				{
					//Modify
					handler->callback(LV_FILEWATCHER_EVENT_MODIFY, data.path.c_str(), nullptr);
				}
				break;
			}

			case FILE_ACTION_RENAMED_OLD_NAME:

				break;

			case FILE_ACTION_RENAMED_NEW_NAME:
			{
				//prev가 RENAME_OLD_NAME일수밖에 없다. 
				if (prevData.action == FILE_ACTION_RENAMED_OLD_NAME)
				{
					//https://jira.com2us.com/jira/browse/CSECOTS-5429
					Engine::LvDispatchQueue::Main().Sync([&]() {
						if (Project::LvFileManager::GetFileInfoAtAbsolutePath(data.path.c_str()).isValid)
						{
							handler->callback(LV_FILEWATCHER_EVENT_MODIFY, data.path.c_str(), nullptr);
						}
						else
						{
							//ReName
							handler->callback(LV_FILEWATCHER_EVENT_MOVE, prevData.path.c_str(), data.path.c_str());
						}
					});
				}
				break;
			}

			}

			prevData = data;
		}

		while (directoriesStack.Count() > 0)
		{
			LvFileWatcherEventSet& set = directoriesStack.Pop();
			dir_watcher_listen(set.path.c_str(), handler, LV_FILEWATCHER_EVENT_CREATE, nullptr);
		}
		directoriesSet.Clear();
	}
}

void listen(LvFileWatcherEventHandler* handler, LvList<LvFileWatcherEventSet> datas)
{
	filteredCallback(handler, datas);
}

void watch(void* t)
{
	LvThread* thread = static_cast<LvThread*>(t);

	LvList<LvFileWatcherEventSet> accumul_datas;
	accumul_datas.Reserve(watcherBufferMaxCount);

	size_t last_dataCount = 0;
	size_t equal_loop = 0;
	
	LvEditorContext* editor = lv_editor_get_context();
	

	while (false == g_shutdown)
	{
		LvFileWatcherDataSet& set = s_threads[thread];

		auto eRes = WaitForMultipleObjectsEx(static_cast<DWORD>(set.watcher->eventList.Count()), set.watcher->eventList.data(), false, INFINITE, true);

		if (eRes >= WAIT_OBJECT_0 && eRes < WAIT_OBJECT_0 + set.watcher->eventList.Count())
		{
			auto index = eRes - WAIT_OBJECT_0;

			//Window Watcher Event
			if (index == 0)
			{
				while (true)
				{
					LvList<LvFileWatcherEventSet> datas;
					lv_file_watcher_get_event(set.watcher, &datas);

					for (auto& each : datas)
					{
						accumul_datas.Add(each);
					}

					last_dataCount = accumul_datas.Count();
					equal_loop = 0;

					if (!datas.IsEmpty())
					{
						{
							//Because Modify Event following Immediatly After Create Event.
							lv_thread_sleep(100);
							continue;
						}
					}

					break;
				}

				SetEvent(set.watcher->checkEvent);
			}
			//Custom Loop Check Event.
			else if (index == 1)
			{
				if (last_dataCount > 0 && last_dataCount == accumul_datas.Count())
				{
					equal_loop++;
					SetEvent(set.watcher->checkEvent);
				}

				
				//10번 checkEvent가 들어올동안 아무 이벤트도 발생하지 않았다면, listen 호출! 
				if (equal_loop > 10 && !set.watcher->pause)
				{
					listen(set.handler, accumul_datas);
					accumul_datas.Clear();
					last_dataCount = 0;
					equal_loop = 0;
				}
			}
		}

	}
}

LvFileWatcher* lv_file_watcher_start(LvFileWatcherCreateFlags flags, LvFileWatcherEventType types, const char* path, LvFileWatcherEventHandler* handler)
{
	g_shutdown = false;

	LvFileWatcher* watcher = lv_file_watcher_create(flags, types, path);

	if (watcher == nullptr)
		return nullptr;

	if (!s_registFileWatcher.ContainsKey(watcher))
	{
		LvThread* thread = new LvThread();
		thread->SetName(path);
		thread->SetPriority(LvThreadPriority::LOW);
		s_threads.Add(thread, LvFileWatcherDataSet{ handler, watcher });
		s_registFileWatcher.Add(watcher, thread);
		thread->Start(watch, (void*)thread);
	}

	return watcher;
}

void lv_file_watcher_end(LvFileWatcher* watcher)
{
	g_shutdown = true;

	if (s_registFileWatcher.ContainsKey(watcher))
	{
		LvThread* thread = s_registFileWatcher[watcher];
		
		SetEvent(watcher->checkEvent);

		thread->Join();
		thread->Stop();

		s_registFileWatcher.Remove(watcher);
		s_threads.Remove(thread);

		delete thread;

		lv_file_watcher_destroy(watcher);
	}
}


void lv_file_watcher_set_pause(const char* path, bool pause)
{
	for (auto& thread : s_threads)
	{
		if (strcmp(thread.key->GetName(), path) == 0)
		{
			thread.value.watcher->pause = pause;

			if (!pause)
			{
				SetEvent(thread.value.watcher->checkEvent);
			}
			break;
		}
	}
}


#undef PRINT_EVENT_LOG
LV_NS_EDITOR_END
