#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

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
	NonCopyable(EncodingDetector);

	bool DetectEncoding(std::string_view sample, Pystykorva::EncodingGuess& encoding);

private:
	EncodingDetectorImpl* _impl;
};