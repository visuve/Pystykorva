#include "PCH.hpp"
#include "BufferedStream.hpp"

BufferedStream::BufferedStream(
	std::istream& input,
	size_t bufferSize,
	uint64_t streamSize) :
	_input(input.rdbuf()),
	_streamSize(streamSize),
	_bytesRead(0)
{
	assert(input);
	assert(bufferSize > 0);
	assert(streamSize > 0);

	if (streamSize < bufferSize)
	{
		bufferSize = streamSize;
	}

	_buffer.resize(bufferSize);
}

BufferedStream::~BufferedStream()
{
}

bool BufferedStream::HasData() const
{
	return _streamSize > 0;
}

bool BufferedStream::Read()
{
	if (_streamSize == 0)
	{
		return false;
	}

	std::streamsize extracted = _input->sgetn(_buffer.data(), _buffer.size());

	if (extracted <= 0)
	{
		_streamSize = 0;
		return false;
	}

	size_t extractedReal = static_cast<size_t>(extracted);

	if (extractedReal < _buffer.size())
	{
		// This should happen only once, i.e. when the last chunk is read
		_buffer.resize(extractedReal);
	}

	_bytesRead += extractedReal;
	_streamSize -= extractedReal;

	return true;
}

std::string_view BufferedStream::Data() const
{
	return _buffer;
}
