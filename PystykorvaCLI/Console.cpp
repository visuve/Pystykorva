#include "PCH.hpp"
#include "Console.hpp"

#if _WIN32
#include <Windows.h>
#include <io.h>

typedef BOOL(*WriteFunctionType)(HANDLE, const void*, DWORD, LPDWORD, LPVOID);

template <WriteFunctionType WriteFunction>
void Write(HANDLE stream, const void* data, size_t size)
{
	assert(data && size);

	DWORD written = 0;

	if (!WriteFunction(stream, data, static_cast<DWORD>(size), &written, nullptr))
	{
		throw std::system_error(GetLastError(), std::system_category(), "WriteConsoleA");
	}

	assert(written > 0);
}

Console::Console(OutputType type) :
	_stream(reinterpret_cast<HANDLE>(_get_osfhandle(type)))
{
	if (_stream == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("Failed to get console handle");
	}
}

Console::~Console()
{
	FlushFileBuffers(_stream);
}

void Console::WriteA(const void* data, size_t size) const
{
	return Write<WriteConsoleA>(_stream, data, size);
}

void Console::WriteW(const void* data, size_t size) const
{
	return Write<WriteConsoleW>(_stream, data, size);
}
#else
#include <unistd.h>

Console::Console(OutputType type)
{
	*_stream = type
}

Console::~Console()
{
	fdatasync(*_stream);
}

void Console::WriteA(const void* data, size_t size) const
{
	assert(data && size);

	ssize_t written = write(*_stream, data, size);

	if (written == -1)
	{
		throw std::system_error(errno, std::system_category(), "write");
	}

	assert(written > 0);
}

void Console::WriteW(const void* data, size_t size) const
{
	return Console::WriteA(data, size * 2);
}
#endif