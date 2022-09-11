#pragma once

class BufferedStream
{
public:
	BufferedStream(
		std::istream& input,
		size_t bufferSize,
		uint64_t streamSize);

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
	uint64_t _streamSize;
	std::string _buffer;
	uint64_t _bytesRead;
};

