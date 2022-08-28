#pragma once

#include "Pystykorva.hpp"

class TextSearcherImpl;

class TextSearcher
{
public:
	struct Result
	{
		size_t Start = 0;
		size_t End = 0;
	};

	TextSearcher(std::u16string_view expression, Pystykorva::MatchMode mode);
	TextSearcher(std::string_view expression, Pystykorva::MatchMode mode);
	~TextSearcher();

	std::vector<Result> FindIn(std::u16string_view sentence);
private:
	TextSearcherImpl* _impl;
};

