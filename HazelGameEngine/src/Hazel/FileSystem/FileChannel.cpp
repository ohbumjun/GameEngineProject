#include "hzpch.h"
#include "FileChannel.h"
#include "FileInfo.h"
#include "DirectorySystem.h"
FileChannel::FileChannel(HANDLE file) : 
	m_FileHandle(file)
{
	m_Position = 0;
	m_Length = 0;

	if (file == INVALID_HANDLE)
	{
		const int errorCode = errno;
		throw("Code = %i : %s", errorCode, DirectorySystem::PrintError(errorCode));
	}

	DirectorySystem::SeekFile(m_FileHandle, 0, FilePosMode::END);
	
	int64 length = 0;
	
	if (DirectorySystem::GetFilePos(m_FileHandle, length))
	{
		m_Length = static_cast<size_t>(length);
	}

	DirectorySystem::SeekFile(m_FileHandle, 0, FilePosMode::BEGIN);

}

FileChannel::FileChannel(const char* path, FileOpenMode mode) 
	: FileChannel(DirectorySystem::pr_file_open(path, FileAccessMode::READ_WRITE, mode))
{
}

FileChannel::FileChannel(const char* path, FileOpenMode mode, FileAccessMode access)
{
}

FileChannel::~FileChannel()
{
}

int64 FileChannel::GetCurrentPos()
{
	return int64();
}

void FileChannel::SetPos(int64 pos)
{
}

void FileChannel::SerializeData(const void* ptr, size_t size)
{
}

void FileChannel::DeserializeData(void* ptr, size_t size)
{
}

void FileChannel::CloseChannel()
{
}

size_t FileChannel::GetDataLength() const
{
	return size_t();
}
