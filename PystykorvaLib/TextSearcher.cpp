#include "PCH.hpp"
#include "TextSearcher.hpp"

class TextSearcherImpl
{
public:
	TextSearcherImpl(std::u16string_view expression, Pystykorva::MatchMode mode)
	{
		int32_t flags;

		switch (mode)
		{
			case Pystykorva::PlainCaseSensitive:
				flags = UREGEX_LITERAL;
				break;
			case Pystykorva::PlainCaseInsensitive:
				flags = UREGEX_CASE_INSENSITIVE | UREGEX_LITERAL;
				break;
			case Pystykorva::RegexCaseSensitive:
				flags = 0;
				break;
			case Pystykorva::RegexCaseInsensitive:
				flags = UREGEX_CASE_INSENSITIVE;
				break;
			default:
				std::unreachable();
		}

		UParseError error;

		_regex = uregex_open(
			expression.data(),
			static_cast<int32_t>(expression.size()),
			flags,
			&error,
			&_status);

		assert(U_SUCCESS(_status));
	}

	~TextSearcherImpl()
	{
		if (_regex)
		{
			uregex_close(_regex);
		}
	}

	std::vector<TextSearcher::Result> FindIn(std::u16string_view sentence)
	{
		std::vector<TextSearcher::Result> results;

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

std::vector<TextSearcher::Result> TextSearcher::FindIn(std::u16string_view sentence)
{
	return _impl->FindIn(sentence);
}
