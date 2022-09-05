#pragma once

class EncodingDetectorImpl;

struct EncodingException : std::runtime_error
{
	inline EncodingException(const std::string& message) :
		std::runtime_error(message)
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