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
	void FindAll(BufferedStream& stream, std::vector<Pystykorva::Match>& matches, Pystykorva::EncodingGuess& encoding);
	Pystykorva::Match ProcessLine(uint64_t offset, uint32_t lineNumber, std::u16string_view line);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

