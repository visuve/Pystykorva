#pragma once

class __declspec(dllexport) Pystykorva
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

	Pystykorva(const Options& options);
	~Pystykorva();

	void Run();

private:
	Options _options;
};