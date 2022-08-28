#pragma once

#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "LineAnalyzer.hpp"
#include "TextSearcher.hpp"

class Puukko
{
public:
	struct Result
	{
		std::u16string Content;
		std::vector<TextSearcher::Result> Hits;
	};

	Puukko(const Pystykorva::Options& options, std::stop_token);
	~Puukko();

	std::map<uint32_t, Puukko::Result> ProcessFile(const std::filesystem::path&);
	std::vector<TextSearcher::Result> ProcessLine(std::u16string_view& line);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
	TextSearcher _textSearcher;
};

