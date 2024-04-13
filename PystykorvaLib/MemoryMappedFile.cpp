#include "PCH.hpp"
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
	MemoryMappedFileImpl(const std::filesystem::path& path, uint64_t fileSize) :
		_file(CreateFileW(
			path.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL))
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
			PAGE_READWRITE,
			mappingSize.HighPart,
			mappingSize.LowPart,
			nullptr);

		if (!_mapping)
		{
			throw IOException("CreateFileMappingW");
		}

		_view = MapViewOfFile(_mapping, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);

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

	std::string_view Data() const
	{
		return { reinterpret_cast<char*>(_view), _size };
	}

	std::string_view Sample(size_t size) const
	{
		return { reinterpret_cast<char*>(_view), std::min(size, _size) };
	}

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
	MemoryMappedFileImpl(const std::filesystem::path& path, uint64_t fileSize) :
		_descriptor(open(path.c_str(), O_RDONLY))
	{
		if (_descriptor == -1)
		{
			throw IOException("open");
		}

		_view = mmap(nullptr, _size, PROT_READ, MAP_PRIVATE, _descriptor, 0);

		if (_view == MAP_FAILED)
		{
			throw IOException("mmap");
		}

		_size = fileSize;
	}

	~MemoryMappedFileImpl()
	{
		if (_descriptor)
		{
			::close(_descriptor);
		}
	}

	NonCopyable(MemoryMappedFileImpl);

	std::string_view Sample(size_t size)
	{
		return { reinterpret_cast<char*>(_view), std::min(_size, size) };
	}

	std::string_view Data() const
	{
		return { reinterpret_cast<char*>(_view), _size };
	}

private:
	int _descriptor = 0;
	void* _view = nullptr;
	uint64_t _size = 0;
};
#endif

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& path, uint64_t fileSize) :
	_impl(new MemoryMappedFileImpl(path, fileSize))
{
}

MemoryMappedFile::~MemoryMappedFile()
{
	delete _impl;
}

std::string_view MemoryMappedFile::Sample(size_t size) const
{
	return _impl->Sample(size);
}

std::string_view MemoryMappedFile::Data() const
{
	return _impl->Data();
}
