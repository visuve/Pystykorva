#pragma once

#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "UnicodeConverter.hpp"
#include "LineAnalyzer.hpp"
#include "TextSearcher.hpp"
#include "BufferedStream.hpp"

class TextProcessor
{
public:
	TextProcessor(const Pystykorva::Options& options, std::stop_token);
	~TextProcessor();

	Pystykorva::Result ProcessFile(const std::filesystem::path&);
	void ProcessStream(std::vector<Pystykorva::Match>& matches, BufferedStream& stream);
	Pystykorva::Match ProcessLine(std::u16string_view line, uint32_t number);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

