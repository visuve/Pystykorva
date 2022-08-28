#pragma once

#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "UnicodeConverter.hpp"
#include "LineAnalyzer.hpp"
#include "TextSearcher.hpp"

class TextProcessor
{
public:
	struct Result
	{
		std::u16string Content;
		std::vector<TextSearcher::Result> Hits;
	};

	TextProcessor(const Pystykorva::Options& options, std::stop_token);
	~TextProcessor();

	std::map<uint32_t, TextProcessor::Result> ProcessFile(const std::filesystem::path&);
	std::map<uint32_t, TextProcessor::Result> ProcessStream(std::iostream& stream);
	TextProcessor::Result ProcessLine(std::u16string_view& line);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

