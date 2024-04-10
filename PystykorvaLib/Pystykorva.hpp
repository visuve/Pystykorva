#pragma once

class Pystykorva
{
public:
	enum MatchMode : uint8_t
	{
		PlainCaseInsensitive,
		PlainCaseSensitive,
		RegexCaseInsensitive,
		RegexCaseSensitive
	};

	struct Options
	{
		std::filesystem::path Directory;
		std::set<std::string> IncludeWildcards;
		std::set<std::string> ExcludedDirectories;

		std::u16string SearchExpression;
		std::u16string ReplacementText;

		MatchMode Mode = PlainCaseInsensitive;

		uint64_t MinimumSize = 0;
		uint64_t MaximumSize = 0;

		std::chrono::time_point<std::chrono::file_clock> MinimumTime;
		std::chrono::time_point<std::chrono::file_clock> MaximumTime;

		uint32_t BufferSize = 0;

		// Zero will default to std::thread::hardware_concurrency or 1
		uint32_t MaximumThreads = 0;
	};

	enum Status : uint32_t
	{
		Ok = 0x00,
		Missing = (1u << 0),
		NameExcluded = (1u << 1),
		NoPermission = (1u << 2),
		TooSmall = (1u << 3),
		TooBig = (1u << 4),
		TooEarly = (1u << 5),
		TooLate = (1u << 6),
		UnknownEncoding = (1u << 7),
		ConversionError = (1u << 8),
		IOError = (1u << 9)
	};

	struct FilePosition
	{
		static constexpr auto Unknown = std::numeric_limits<size_t>::max();

		size_t Begin = 0; // Offset from the beginning of the file
		size_t End = Unknown; // Offset from the beginning of the file

		constexpr bool operator == (const FilePosition& other) const
		{
			return Begin == other.Begin && End == other.End;
		}

		constexpr auto operator <=> (const FilePosition& other) const = default;
	};

	struct Match
	{
		uint32_t LineNumber = 0;
		std::vector<FilePosition> Positions;
		std::u16string Content;

		constexpr bool operator == (const Match& other) const
		{
			return LineNumber == other.LineNumber &&
				Positions == other.Positions &&
				Content == other.Content;
		}
	};

	struct Result
	{
		uint32_t StatusMask = 0;
		std::vector<Match> Matches;
	};

	struct Callbacks
	{
		std::function<void()> Started;
		std::function<void(const std::filesystem::path&)> Processing;
		std::function<void(const std::filesystem::path&, const Result&)> Processed;
		std::function<void(std::chrono::milliseconds)> Finished;
	};

	Pystykorva(const Options& options, const Callbacks& callbacks);
	~Pystykorva();

	void Start();
	void Wait();
	void Stop();

private:
	bool IsExcludedDirectory(const std::filesystem::path&) const;
	std::filesystem::path Next();
	void Worker(std::stop_token token);

	Options _options;
	Callbacks _callbacks;

	std::chrono::high_resolution_clock::time_point _start;
	std::vector<std::jthread> _threads;
	std::filesystem::recursive_directory_iterator _rdi;
	std::mutex _mutex;
};