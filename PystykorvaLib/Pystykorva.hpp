#pragma once

class Pystykorva
{
public:
	enum MatchMode : uint8_t
	{
		PlainCaseSensitive = 0,
		PlainCaseInsensitive,
		RegexCaseSensitive,
		RegexCaseInsensitive
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

		// Zero will default to std::thread::hardware_concurrency or 1
		uint32_t MaximumThreads = 0;
	};

	struct IFile
	{
		virtual std::string_view Sample(size_t size = 0x400) const = 0;
		virtual std::string_view Data() const = 0;
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
		EncodingError = (1u << 7),
		ConversionError = (1u << 8),
		AnalysisError = (1u << 9),
		SearchError = (1u << 10),
		IOError = (1u << 11)
	};

	struct Position
	{
		static constexpr auto Unknown = std::numeric_limits<uint64_t>::max();

		constexpr Position(uint64_t begin = 0, uint64_t end = Unknown) :
			Begin(begin),
			End(end)
		{
		}

		constexpr uint64_t Size() const
		{
			return End - Begin;
		}

		constexpr bool operator == (const Position& other) const
		{
			return Begin == other.Begin && End == other.End;
		}

		uint64_t Begin;
		uint64_t End;
	};

	struct RelAbsPosPair
	{
		constexpr RelAbsPosPair(
			uint64_t relativeBegin,
			uint64_t relativeEnd,
			uint64_t absoluteBegin,
			uint64_t absoluteEnd) :
			Relative(relativeBegin, relativeEnd),
			Absolute(absoluteBegin, absoluteEnd)
		{
		}

		constexpr RelAbsPosPair(Position relative, uint64_t offset) :
			Relative(relative),
			Absolute(relative.Begin + offset, relative.End + offset)
		{
		}

		constexpr bool operator == (const RelAbsPosPair& other) const
		{
			return Relative == other.Relative && Absolute == other.Absolute;
		}

		Position Relative;
		Position Absolute;
	};

	struct Match
	{
		uint32_t LineNumber = 0;
		std::u16string LineContent;
		std::vector<RelAbsPosPair> Positions;
	};

	struct EncodingGuess
	{
		static constexpr char Unknown[] = "unknown";
		int32_t Confidence = 0;
		std::string Name = Unknown;
	};

	struct Result
	{
		uint32_t StatusMask = Status::Ok;
		EncodingGuess Encoding;
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