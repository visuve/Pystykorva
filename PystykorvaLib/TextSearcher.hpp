#pragma once

#include "Pystykorva.hpp"

class TextSearcherImpl;

class TextSearcher
{
public:
	TextSearcher(std::u16string_view expression, Pystykorva::MatchMode mode);
	TextSearcher(std::string_view expression, Pystykorva::MatchMode mode);
	~TextSearcher();

	std::vector<Pystykorva::Match> FindIn(std::u16string_view sentence);
private:
	TextSearcherImpl* _impl;
};

