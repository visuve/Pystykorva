#pragma once

class BufferedStream
{
public:
	BufferedStream(
		std::istream& input,
		std::streamsize bufferSize,
		std::streamsize streamSize);

	BufferedStream(const BufferedStream&) = delete;
	BufferedStream(BufferedStream&&) = delete;
	BufferedStream& operator = (const BufferedStream&) = delete;
	BufferedStream& operator = (BufferedStream&&) = delete;

	~BufferedStream();

	bool HasData() const;
	bool Read();
	std::string_view Data() const;
	uint64_t Offset() const;

private:
	std::streambuf* _input;
	std::streamsize _streamSize;
	std::string _buffer;
	std::streamsize _bytesRead;
};

