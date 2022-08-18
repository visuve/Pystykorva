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

		uint64_t MinimumSize = 0;
		uint64_t MaximumSize = 0;

		std::chrono::time_point<std::chrono::file_clock> MinimumTime;
		std::chrono::time_point<std::chrono::file_clock> MaximumTime;

		uint16_t MaximumThreads = 0;
	};

	enum Status : uint8_t
	{
		Ok,
		Excluded,
		TooSmall,
		TooBig,
		TooEarly,
		TooLate
	};

	struct Callbacks
	{
		std::function<void()> Started;
		std::function<void(std::filesystem::path)> Processing;
		std::function<void(uint32_t line, std::string content)> MatchFound;
		std::function<void(std::filesystem::path, Status)> Processed;
		std::function<void(std::chrono::milliseconds)> Finished;
	};

	Pystykorva(const Options& options, const Callbacks& callbacks);
	~Pystykorva();

	void Start();
	void Stop();

private:
	Options _options;
	Callbacks _callbacks;

	std::unique_ptr<boost::asio::thread_pool> _pool;
	std::atomic_bool _run;
};