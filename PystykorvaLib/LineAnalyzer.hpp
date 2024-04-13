#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

class LineAnalyzerImpl;

struct AnalysisException : std::runtime_error
{
	inline AnalysisException(const std::string& message) :
		std::runtime_error(message)
	{
	}
};

class LineAnalyzer
{
public:
	LineAnalyzer();
	~LineAnalyzer();
	NonCopyable(LineAnalyzer);

	std::vector<Pystykorva::Position> Boundaries(std::u16string_view sample);

private:
	LineAnalyzerImpl* _impl;
};
