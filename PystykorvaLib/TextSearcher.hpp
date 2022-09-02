#pragma once

#include "Pystykorva.hpp"

struct SearchException : std::exception
{
	inline SearchException(const char* message) :
		std::exception(message)
	{
	}
};

class TextSearcherImpl;

class TextSearcher
{
public:
	TextSearcher(std::u16string_view expression, Pystykorva::MatchMode mode);
	TextSearcher(std::string_view expression, Pystykorva::MatchMode mode);
	~TextSearcher();

	std::vector<Pystykorva::MatchPosition> FindIn(std::u16string_view sentence);
private:
	TextSearcherImpl* _impl;
};

