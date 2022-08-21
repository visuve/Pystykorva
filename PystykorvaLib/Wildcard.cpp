#include "PCH.hpp"
#include "Wildcard.hpp"

inline bool IEquals(char lhs, char rhs)
{
	// https://en.cppreference.com/w/cpp/string/byte/tolower#Notes
	return std::tolower(static_cast<uint8_t>(lhs)) == 
		std::tolower(static_cast<uint8_t>(rhs));
}

bool Wildcard::Matches(std::string_view text, std::string_view wildcard)
{
	std::string_view::iterator textNext = text.begin();
	std::string_view::iterator textPrev;

	std::string_view::iterator wildNext = wildcard.begin();
	std::string_view::iterator wildPrev  = wildcard.end();

	while (textNext != text.end() && wildNext != wildcard.end())
	{
		if (*wildNext == '*')
		{
			textPrev = textNext;
			wildPrev = ++wildNext;
		}
		else if (*wildNext == '?' || IEquals(*wildNext, *textNext))
		{
			textNext++;
			wildNext++;
		}
		else if (wildPrev == wildcard.end())
		{
			return false;
		}
		else
		{
			textNext = ++textPrev;
			wildNext = wildPrev;
		}
	}

	while (wildNext != wildcard.end() && *wildNext == '*')
	{
		++wildNext;
	}

	return wildNext == wildcard.end() ? true : false;
}
