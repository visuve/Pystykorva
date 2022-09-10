#include "PCH.hpp"
#include "BufferedStream.hpp"

BufferedStream::BufferedStream(
	std::istream& input,
	std::streamsize bufferSize,
	std::streamsize streamSize) :
	_input(input),
	_streamSize(streamSize)
{
	assert(input);
	assert(bufferSize > 0);
	assert(streamSize > 0);

	_buffer.resize(static_cast<size_t>(bufferSize), '\0');
}

BufferedStream::~BufferedStream()
{
}

bool BufferedStream::HasData()
{
	return _streamSize > 0;
}

bool BufferedStream::Read()
{
	if (_streamSize <= 0)
	{
		return false;
	}

	std::streamsize extracted = _input.read(_buffer.data(), _buffer.size()).gcount();

	if (extracted < static_cast<std::streamsize>(_buffer.size()))
	{
		// This should happen only once, i.e. when the last chunk is read
		_buffer.resize(static_cast<size_t>(extracted));
	}

	_streamSize -= extracted;

	return extracted > 0;
}

std::string_view BufferedStream::Data() const
{
	return _buffer;
}