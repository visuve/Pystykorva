#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "UnicodeConverter.hpp"
#include "LineAnalyzer.hpp"
#include "TextSearcher.hpp"

class TextProcessor
{
public:
	TextProcessor(std::stop_token, const Pystykorva::Options&);
	~TextProcessor();
	NonCopyable(TextProcessor);

	Pystykorva::Result ProcessPath(const std::filesystem::path&);
	void ProcessFile(Pystykorva::IFile& file, std::vector<Pystykorva::Match>& matches, std::string_view encoding);
	Pystykorva::Match ProcessLine(uint64_t offset, uint32_t lineNumber, std::u16string_view line);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

