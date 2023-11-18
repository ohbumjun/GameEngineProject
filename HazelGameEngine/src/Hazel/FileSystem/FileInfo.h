#pragma once

#include "hzpch.h"

enum class FileAccessMode
{
	READ_ONLY,	
	WRITE_ONLY,	
	READ_WRITE	
};

enum class FileOpenMode : uint8
{
	OPEN_CREATE,	// 항상 열기. 없으면 만들어서 열기
	OPEN,				// 있을 때만 열기
	CREATE,			// 있어도 생성. 항상 생성
	APPEND,			// 없으면 생성
	NEW,					// 있을 때만 열기, 없으면 에러
};

enum class FilePosMode
{
	BEGIN,		
	CURRENT,	
	END			
};

struct FileInfo
{
	std::string m_Name;

	size_t m_Size = 0;

	bool m_ReadOnly = false;

	time_t m_LastEditTime = 0;

	time_t m_CreateTime = 0;
};
