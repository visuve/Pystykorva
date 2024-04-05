#include "PCH.hpp"
#include "Console.hpp"

#if _WIN32
#include <Windows.h>
#include <io.h>

Console::Console(OutputType type)
{
	_stream = reinterpret_cast<HANDLE>(_get_osfhandle(type));
	assert(_stream != INVALID_HANDLE_VALUE);
}

Console::~Console()
{
	if (_stream)
	{
		FlushFileBuffers(_stream);
	}
}

void Console::WriteA(const void* data, size_t size) const
{
	assert(data);

	DWORD written = 0;

	if (!WriteConsoleA(_stream, data, static_cast<DWORD>(size), &written, nullptr))
	{
		throw std::system_error(GetLastError(), std::system_category(), "WriteConsoleA");
	}

	assert(written > 0);
}

void Console::WriteW(const void* data, size_t size) const
{
	assert(data);

	DWORD written = 0;

	if (!WriteConsoleW(_stream, data, static_cast<DWORD>(size), &written, nullptr))
	{
		throw std::system_error(GetLastError(), std::system_category(), "WriteConsoleW");
	}

	assert(written > 0);
}
#endif