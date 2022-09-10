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

	bool HasData();
	bool Read();
	std::string_view Data() const;

private:
	std::istream& _input;
	std::streamsize _streamSize;
	std::string _buffer;
};

