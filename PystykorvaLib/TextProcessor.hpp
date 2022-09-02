#pragma once

#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "UnicodeConverter.hpp"
#include "LineAnalyzer.hpp"
#include "TextSearcher.hpp"

class TextProcessor
{
public:
	TextProcessor(const Pystykorva::Options& options, std::stop_token);
	~TextProcessor();

	Pystykorva::Result ProcessFile(const std::filesystem::path&);
	void ProcessStream(std::vector<Pystykorva::Match>& matches, std::iostream& stream, std::streamsize size = -1);
	Pystykorva::Match ProcessLine(std::u16string_view line, uint32_t number);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

