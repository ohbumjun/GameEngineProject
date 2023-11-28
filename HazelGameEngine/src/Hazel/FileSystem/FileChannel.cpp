#include "hzpch.h"
#include "FileChannel.h"
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
	: FileChannel(DirectorySystem::OpenFile(path, FileAccessMode::READ_WRITE, mode))
{
}

FileChannel::FileChannel(const char* path, FileOpenMode mode, FileAccessMode access) 
	: FileChannel(DirectorySystem::OpenFile(path, access, mode))
{
}

FileChannel::~FileChannel()
{
	if (m_FileHandle != INVALID_HANDLE)
	{
		DirectorySystem::CloseFile(m_FileHandle);
		m_FileHandle = INVALID_HANDLE;
	}
}

int64 FileChannel::GetCurrentPos()
{
	int64 position = 0;

	//if (fgetpos(_file.handle, &_position) != 0)
	if (!DirectorySystem::GetFilePos(m_FileHandle, position))
	{
		const int errorCode = errno;

		if (errorCode != 0)
		{
			THROW("File getpos failed Code = %i : %s", errorCode, DirectorySystem::PrintError(errorCode));
		}
	}

	m_Position = static_cast<size_t>(position);

	return position;
}

void FileChannel::SetPos(int64 pos)
{
	m_Position = static_cast<size_t>(pos);

	//if (fsetpos(_file.handle, &_position) != 0)
	if (!DirectorySystem::SetFilePos(m_FileHandle, pos))
	{
		const int errorCode = errno;
		if (errorCode != 0)
		{
			THROW("File setpos failed Code = %i : %s", errorCode, DirectorySystem::PrintError(errorCode));
		}
	}
}

void FileChannel::SerializeData(const void* ptr, size_t size)
{
	DirectorySystem::WriteToFile(m_FileHandle, ptr, size);
	//fwrite(ptr, size, 1, _file.handle);
	m_Position += size;
	m_Length = std::max(m_Position, static_cast<size_t>(m_Length + 1));

}

void FileChannel::DeserializeData(void* ptr, size_t size)
{
	if (size == 0) THROW("size > 0 Size should not be zero");

	uint8* movePointer = static_cast<uint8*>(ptr);
	size_t leftOverReadSize = size;
	size_t currentReadSize = 0;
	size_t totalReadSize = 0;
	while (totalReadSize < size)
	{
		//currentReadSize = fread(movePointer, 1, leftOverReadSize, _file.handle);
		currentReadSize = DirectorySystem::ReadFromFile(m_FileHandle, movePointer, leftOverReadSize);
		totalReadSize += currentReadSize;
		if (totalReadSize >= size) break;

		movePointer += currentReadSize;
		leftOverReadSize -= currentReadSize;

		if (/*feof(_file.handle)*/ DirectorySystem::IsFileEOF(m_FileHandle) || currentReadSize <= 0)
		{
			// Fail to read for some problems
			break;
		}
	}

	m_Position += totalReadSize;

	if (totalReadSize != size)
	{
		const int errorCode = errno;
		if (errorCode != 0)
		{
			THROW("File read failed Code = %i : %s", errorCode, DirectorySystem::PrintError(errorCode));
		}
	}
}

void FileChannel::FlushToFile() const
{
	DirectorySystem::FlushFile(m_FileHandle);
}

void FileChannel::CloseChannel()
{
	DirectorySystem::CloseFile(m_FileHandle);
	m_FileHandle = INVALID_HANDLE;
}

size_t FileChannel::GetDataLength() const
{
	return m_Length;
}
