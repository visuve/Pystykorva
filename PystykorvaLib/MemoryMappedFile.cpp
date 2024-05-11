#include "PystykorvaLib.pch"
#include "MemoryMappedFile.hpp"

#ifdef _WIN32

#define NOMINMAX
#include <Windows.h>

IOException::IOException(const std::string& message) :
	std::system_error(GetLastError(), std::system_category(), message)
{
}

class MemoryMappedFileImpl
{
public:
	MemoryMappedFileImpl(const std::filesystem::path& path, uint64_t fileSize, bool readOnly) :
		_file(CreateFileW(
			path.c_str(),
			readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			readOnly ? OPEN_EXISTING : CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr))
	{
		if (!_file || _file == INVALID_HANDLE_VALUE)
		{
			throw IOException("CreateFileW");
		}

		LARGE_INTEGER mappingSize;
		mappingSize.QuadPart = fileSize;

		_mapping = CreateFileMappingW(
			_file,
			nullptr,
			readOnly ? PAGE_READONLY : PAGE_READWRITE,
			mappingSize.HighPart,
			mappingSize.LowPart,
			nullptr);

		if (!_mapping)
		{
			throw IOException("CreateFileMappingW");
		}

		_view = MapViewOfFile(
			_mapping,
			readOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS,
			0,
			0,
			fileSize);

		if (!_view)
		{
			throw IOException("MapViewOfFile");
		}

		_size = fileSize;
	}

	~MemoryMappedFileImpl()
	{
		if (_view)
		{
			UnmapViewOfFile(_view);
		}

		if (_mapping)
		{
			CloseHandle(_mapping);
		}

		if (_file)
		{
			CloseHandle(_file);
		}
	}

	NonCopyable(MemoryMappedFileImpl);
	friend class MemoryMappedFile;

private:
	HANDLE _file = nullptr;
	HANDLE _mapping = nullptr;
	void* _view = nullptr;
	uint64_t _size = 0;
};
#else

IOException::IOException(const std::string& message) :
	std::system_error(errno, std::system_category(), message)
{
}

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

class MemoryMappedFileImpl
{
public:
	MemoryMappedFileImpl(const std::filesystem::path& path, uint64_t fileSize, bool readOnly) :
		_descriptor(open(path.c_str(), readOnly ? O_RDONLY : O_RDWR))
	{
		if (_descriptor == -1)
		{
			throw IOException("open");
		}

		_view = mmap(
			nullptr,
			_size,
			readOnly ? PROT_READ : PROT_READ | PROT_WRITE,
			MAP_PRIVATE,
			_descriptor,
			0);

		if (_view == MAP_FAILED)
		{
			throw IOException("mmap");
		}

		_size = fileSize;
	}

	~MemoryMappedFileImpl()
	{
		if (_view)
		{
			munmap(_view, _size);
		}

		if (_descriptor)
		{
			close(_descriptor);
		}
	}

	NonCopyable(MemoryMappedFileImpl);
	friend class MemoryMappedFile;
private:
	int _descriptor = 0;
	void* _view = nullptr;
	uint64_t _size = 0;
};
#endif

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& path, uint64_t fileSize, bool readOnly) :
	_impl(new MemoryMappedFileImpl(path, fileSize, readOnly))
{
}

MemoryMappedFile::~MemoryMappedFile()
{
	delete _impl;
}

uint64_t MemoryMappedFile::Size() const
{
	return _impl->_size;
}

std::string_view MemoryMappedFile::Sample(size_t size) const
{
	size = std::min(_impl->_size, size);
	return { reinterpret_cast<char*>(_impl->_view), size };
}

std::string_view MemoryMappedFile::Chunk(uint64_t offset, uint64_t size) const
{
	if (offset + size > _impl->_size)
	{
		throw std::out_of_range("chunk out of bounds");
	}

	return { reinterpret_cast<char*>(_impl->_view) + offset, size };
}

std::string_view MemoryMappedFile::Data() const
{
	return { reinterpret_cast<char*>(_impl->_view), _impl->_size };
}

void MemoryMappedFile::Read(void* data, uint64_t size)
{
	if (_offset + size > _impl->_size)
	{
		throw std::out_of_range("read out of bounds");
	}

	for (uint64_t offset = 0; offset < size; ++offset, ++_offset)
	{
		auto source = reinterpret_cast<const uint8_t*>(_impl->_view) + _offset;
		auto target = reinterpret_cast<uint8_t*>(data) + offset;
		*target = *source;
	}
}

void MemoryMappedFile::Write(const void* data, uint64_t size)
{
	if (_offset + size > _impl->_size)
	{
		throw std::out_of_range("write out of bounds");
	}

	for (uint64_t offset = 0; offset < size; ++offset, ++_offset)
	{
		auto source = reinterpret_cast<const uint8_t*>(data) + offset;
		auto target = reinterpret_cast<uint8_t*>(_impl->_view) + _offset;
		*target = *source;
	}
}