#include "PystykorvaLib.pch"
#include "Wildcard.hpp"

// It's a shame that std::tolower is not constexpr :(
inline bool IEquals(char lhs, char rhs)
{
	// https://en.cppreference.com/w/cpp/string/byte/tolower#Notes
	return std::tolower(static_cast<uint8_t>(lhs)) == 
		std::tolower(static_cast<uint8_t>(rhs));
}

bool Wildcard::Matches(std::string_view text, std::string_view wildcard)
{
	// Adapted from:
	// https://github.com/keineahnung2345/leetcode-cpp-practices/blob/master/44.%20Wildcard%20Matching.cpp

	static constexpr size_t Max = std::numeric_limits<size_t>::max();

	size_t textIter = 0, wildIter = 0;
	size_t lastMatch = Max, star = Max;

	while (textIter < text.size()) {

		if (wildIter < wildcard.size() && (IEquals(wildcard[wildIter], text[textIter]) || wildcard[wildIter] == '?'))
		{
			textIter++;
			wildIter++;
		}
		else if (wildIter < wildcard.size() && wildcard[wildIter] == '*')
		{
			star = wildIter;
			lastMatch = textIter;
			wildIter++;
		}
		else if (star != Max)
		{
			lastMatch++;
			textIter = lastMatch;
			wildIter = star + 1;
		}
		else
		{
			return false;
		}
	}

	return wildcard.substr(wildIter) == std::string(wildcard.size() - wildIter, '*');
}
