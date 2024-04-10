#include "PCH.hpp"
#include "TextSearcher.hpp"
#include "UnicodeConverter.hpp"

constexpr int32_t ModeToFlags(Pystykorva::MatchMode mode)
{
	switch (mode)
	{
		case Pystykorva::PlainCaseSensitive:
			return UREGEX_LITERAL;
			break;
		case Pystykorva::PlainCaseInsensitive:
			return UREGEX_CASE_INSENSITIVE | UREGEX_LITERAL;
			break;
		case Pystykorva::RegexCaseSensitive:
			return 0;
			break;
		case Pystykorva::RegexCaseInsensitive:
			return UREGEX_CASE_INSENSITIVE;
			break;
	}

	// std::unreachable();
	throw std::invalid_argument("Unknown mode");
}

class TextSearcherImpl
{
public:
	TextSearcherImpl(std::u16string_view expression, Pystykorva::MatchMode mode)
	{
		UParseError error;

		_regex = uregex_open(
			expression.data(),
			static_cast<int32_t>(expression.size()),
			ModeToFlags(mode),
			&error,
			&_status);

		if (U_FAILURE(_status))
		{
			throw SearchException("uregex_open failed");
		}
	}

	~TextSearcherImpl()
	{
		if (_regex)
		{
			uregex_close(_regex);
		}
	}

	std::vector<Pystykorva::FilePosition> FindIn(std::u16string_view sentence)
	{
		std::vector<Pystykorva::FilePosition> results;

		uregex_setText(_regex,
			sentence.data(),
			static_cast<int32_t>(sentence.size()),
			&_status);

		while (U_SUCCESS(_status) && uregex_findNext(_regex, &_status))
		{
			int32_t start = uregex_start(_regex, 0, &_status);
			assert(U_SUCCESS(_status) && start >= 0);

			int32_t end = uregex_end(_regex, 0, &_status);
			assert(U_SUCCESS(_status) && end > 0);

			results.emplace_back(
				static_cast<size_t>(start),
				static_cast<size_t>(end));
		}

		return results;
	}

private:
	UErrorCode _status = U_ZERO_ERROR;
	URegularExpression* _regex = nullptr;
};

TextSearcher::TextSearcher(std::u16string_view expression, Pystykorva::MatchMode mode) :
	_impl(new TextSearcherImpl(expression, mode))
{
}

TextSearcher::~TextSearcher()
{
	delete _impl;
}

std::vector<Pystykorva::FilePosition> TextSearcher::FindIn(std::u16string_view sentence)
{
	return _impl->FindIn(sentence);
}
