#pragma once

class __declspec(dllexport) BoostGrep
{
public:
	enum MatchMode : uint8_t
	{
		PlainCaseSensitive,
		PlainCaseInsensitive,
		RegexCaseSensitive,
		RegexCaseInsensitive
	};

	struct Options
	{
		std::filesystem::path Directory;
		std::set<std::string> Wildcards;
		std::set<std::string> Excludes;

		std::string SearchExpression;
		std::string ReplacementText;

		MatchMode Mode;

		uint64_t MinimumSize;
		uint64_t MaximumSize;

		std::chrono::time_point<std::chrono::file_clock> MinimumTime;
		std::chrono::time_point<std::chrono::file_clock> MaximumTime;
	};

	BoostGrep(const Options& options);
	~BoostGrep();

	void Run();

private:
	Options _options;
};