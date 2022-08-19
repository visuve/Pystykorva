#pragma once

class Pystykorva
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

		uint32_t MaximumThreads = 0;
	};

	enum Status : uint32_t
	{
		Ok = 0x00,
		Missing = (1u << 0),
		NoPermission = (1u << 1),
		NameExcluded = (1u << 2),
		TooSmall = (1u << 3),
		TooBig = (1u << 4),
		TooEarly = (1u << 5),
		TooLate = (1u << 6)
	};

	struct Callbacks
	{
		std::function<void()> Started;
		std::function<void(std::filesystem::path)> Processing;
		std::function<void(std::filesystem::path, std::map<uint32_t, std::string>, uint32_t statusMask)> Processed;
		std::function<void(std::chrono::milliseconds)> Finished;
	};

	Pystykorva(const Options& options, const Callbacks& callbacks);
	~Pystykorva();

	void Start();
	void Wait();
	void Stop();

	std::filesystem::path Next();

private:
	uint32_t FileStatus(const std::filesystem::path&);
	void Worker(std::stop_token token);

	Options _options;
	Callbacks _callbacks;

	std::vector<std::jthread> _threads;
	std::filesystem::recursive_directory_iterator _rdi;
	std::mutex _mutex;
};