#pragma once

#include "Pystykorva.hpp"

class LineAnalyzerImpl;

class LineAnalyzer
{
public:
	LineAnalyzer();
	~LineAnalyzer();

	std::vector<Pystykorva::FilePosition> Boundaries(std::u16string_view sample);

private:
	LineAnalyzerImpl* _impl;
};
