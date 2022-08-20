#pragma once

#include "Pystykorva.hpp"
#include "EncodingDetector.hpp"
#include "LineAnalyzer.hpp"

class Puukko
{
public:
	Puukko(const Pystykorva::Options& options, std::stop_token);
	~Puukko();

	std::map<uint32_t, std::string> Process(const std::filesystem::path&);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
	EncodingDetector _encodingDetector;
	LineAnalyzer _lineAnalyzer;
};

