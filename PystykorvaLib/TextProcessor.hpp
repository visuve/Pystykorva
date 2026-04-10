#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

class TextProcessor
{
public:
	TextProcessor(std::stop_token, const Pystykorva::Options&);
	~TextProcessor();
	NonCopyable(TextProcessor);

	Pystykorva::Result ProcessPath(const std::filesystem::path&);
	void ProcessFile(std::fstream& file, std::vector<Pystykorva::Match>& matches);

private:
	std::stop_token _token;
	Pystykorva::Options _options;
};

