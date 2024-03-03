#pragma once
#ifndef __LV_FILE_WATCHER_EVENTS_H__
#define __LV_FILE_WATCHER_EVENTS_H__

#include "LvPrecompiled.h"
#include "LvFileWatcher.h"
#include <stddef.h>

template<typename T>
class LvBaseString;

#define LV_FILE_WATCHING_INTERVAL 1000 //ms

#ifdef __cplusplus
extern "C" {
#endif 

LV_NS_EDITOR_BEGIN
enum LvFileWatcherEventType : int;

struct LV_EDITOR_API LvFileWatcherEventHandler
{
	bool(*callback)(LvFileWatcherEventType evtype, const char* src, const char* dst);
};

/**
 @brief 파일/폴더의 변동사항을 추적하는 watcher를 생성하고 실행합니다.
 @details path에 생기는 모든 변동사항을 추적하는 watcher를 생성합니다. 생성 후 바로 감시를 시작합니다.
 @param flags 하위폴더 감시여부
 @param type 추적 변동사항 종류
 @param path 감시 경로
 @param handler 콜백 handler
 @return 생성된 watcher 주소
*/
LV_EDITOR_API
LvFileWatcher* lv_file_watcher_start(LvFileWatcherCreateFlags flags, LvFileWatcherEventType types, const char* path, LvFileWatcherEventHandler* handler);

/**
 @brief 파일/폴더의 변동사항을 추적하는 watcher를 중지하고 삭제합니다.
 @param watcher 감시를 중지할 watcher
*/
LV_EDITOR_API
void lv_file_watcher_end(LvFileWatcher* watcher);

/**
 * @brief
 *
 * @param path filewatcher의 path
 * @param pause pause여부.
 */
LV_EDITOR_API
void lv_file_watcher_set_pause(const char* path, bool pause);

LV_NS_EDITOR_END

#ifdef __cplusplus
}
#endif 

#endif // _LV_FILE_WATCHER_H_
