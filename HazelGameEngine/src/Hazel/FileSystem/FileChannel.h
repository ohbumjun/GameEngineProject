#pragma once

#include "FileInfo.h"
#include "Hazel/Core/Memory/Channel.h"

class FileChannel : public BaseChannel
{

public:
	FileChannel(HANDLE file);

	FileChannel(const char* path, FileOpenMode mode);

	FileChannel(const char* path, FileOpenMode mode, FileAccessMode access);

	~FileChannel() override;

	int64 GetCurrentPos() override;

	void SetPos(int64 pos) override;

	void SerializeData(const void* ptr, size_t size) override;

	void DeserializeData(void* ptr, size_t size) override;

	void CloseChannel() override;

	size_t GetDataLength() const override;

private:
	HANDLE m_FileHandle;
};