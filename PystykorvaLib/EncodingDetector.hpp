#pragma once

class EncodingDetectorImpl;

class EncodingDetector
{
public:
	EncodingDetector();
	~EncodingDetector();

	std::string DetectEncoding(std::string_view sample);

private:
	EncodingDetectorImpl* _impl;
};