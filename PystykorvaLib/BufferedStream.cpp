#include "PCH.hpp"
#include "BufferedStream.hpp"

BufferedStream::BufferedStream(
	std::istream& input,
	std::streamsize bufferSize,
	std::streamsize streamSize) :
	_input(input.rdbuf()),
	_streamSize(streamSize),
	_bytesRead(0)
{
	assert(input);
	assert(bufferSize > 0);
	assert(streamSize > 0);

	_buffer.resize(static_cast<size_t>(bufferSize));
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
	if (_streamSize <= 0)
	{
		return false;
	}

	std::streamsize extracted = _input->sgetn(_buffer.data(), _buffer.size());

	if (extracted < static_cast<std::streamsize>(_buffer.size()))
	{
		// This should happen only once, i.e. when the last chunk is read
		_buffer.resize(static_cast<size_t>(extracted));
	}

	_bytesRead += extracted;
	_streamSize -= extracted;

	return extracted > 0;
}

std::string_view BufferedStream::Data() const
{
	return _buffer;
}

uint64_t BufferedStream::Offset() const
{
	return static_cast<uint64_t>(_bytesRead);
}
