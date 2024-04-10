#pragma once

#include "Pystykorva.hpp"

struct SearchException : std::runtime_error
{
	inline SearchException(const std::string& message) :
		std::runtime_error(message)
	{
	}
};

class TextSearcherImpl;

class TextSearcher
{
public:
	TextSearcher(std::u16string_view expression, Pystykorva::MatchMode mode);
	~TextSearcher();

	std::vector<Pystykorva::FilePosition> FindIn(std::u16string_view sentence);
private:
	TextSearcherImpl* _impl;
};

