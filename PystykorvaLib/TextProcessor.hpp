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

	std::vector<Pystykorva::Result> ProcessFile(const std::filesystem::path&);
	std::vector<Pystykorva::Result> ProcessStream(std::iostream& stream);
	Pystykorva::Result ProcessLine(std::u16string_view& line, uint32_t number);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

