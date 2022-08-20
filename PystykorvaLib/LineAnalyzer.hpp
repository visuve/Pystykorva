#pragma once

class LineAnalyzerImpl;

class LineAnalyzer
{
public:
	struct LineBoundary
	{
		size_t Begin = 0;
		std::optional<size_t> End;
	};

	LineAnalyzer();
	~LineAnalyzer();

	std::vector<LineBoundary> Boundaries(std::u16string_view sample);

private:
	LineAnalyzerImpl* _impl;
};
