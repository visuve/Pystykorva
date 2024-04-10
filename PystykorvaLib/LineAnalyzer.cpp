#include "PCH.hpp"
#include "LineAnalyzer.hpp"

class LineAnalyzerImpl
{
public:
	LineAnalyzerImpl() :
		_iterator(ubrk_open(UBRK_SENTENCE, nullptr, nullptr, 0, &_status))
	{
		assert(U_SUCCESS(_status));
	}

	~LineAnalyzerImpl()
	{
		if (_iterator)
		{
			ubrk_close(_iterator);
		}
	}

	std::vector<Pystykorva::Position> Boundaries(std::u16string_view sample)
	{
		std::vector<Pystykorva::Position> result;

		ubrk_setText(_iterator, sample.data(), static_cast<int32_t>(sample.size()), &_status);
		assert(U_SUCCESS(_status));

		int32_t begin = ubrk_first(_iterator);
		int32_t end = ubrk_next(_iterator);

		while (end != UBRK_DONE)
		{
			if (end == sample.size())
			{
				result.emplace_back(begin);
				break;
			}

			result.emplace_back(begin, end);
			begin = ubrk_current(_iterator);
			end = ubrk_next(_iterator);
		}

		return result;
	}

private:
	UErrorCode _status = U_ZERO_ERROR;
	UBreakIterator* _iterator = nullptr;
};

LineAnalyzer::LineAnalyzer() :
	_impl(new LineAnalyzerImpl())
{
}

LineAnalyzer::~LineAnalyzer()
{
	delete _impl;
}

std::vector<Pystykorva::Position> LineAnalyzer::Boundaries(std::u16string_view sample)
{
	return _impl->Boundaries(sample);
}