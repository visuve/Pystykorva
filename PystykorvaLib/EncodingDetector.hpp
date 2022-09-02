#pragma once

class EncodingDetectorImpl;

struct EncodingException : std::exception
{
	inline EncodingException(const char* message) :
		std::exception(message)
	{
	}
};

class EncodingDetector
{
public:
	EncodingDetector();
	~EncodingDetector();

	std::string DetectEncoding(std::string_view sample);

private:
	EncodingDetectorImpl* _impl;
};