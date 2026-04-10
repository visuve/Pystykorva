#include "PystykorvaCLI.pch"
#include "Pystykorva.hpp"
#include "CmdArgs.hpp"
#include "Console.hpp"

Console Cout(Console::StandardOutput);
Console Cerr(Console::StandardError);

Pystykorva::Options Deserialize(const CmdArgs& args)
{
	Pystykorva::Options options;

	options.Directory = args.Value<std::filesystem::path>("directory");
	options.IncludeWildcards = args.Value<std::set<std::string>>("wildcards");
	options.ExcludedDirectories = args.Value<std::set<std::string>>("excludes");

	options.SearchExpression = args.Value<std::string>("searchexpression");
	options.ReplacementText = args.Value<std::string>("replacement");

	options.Mode = static_cast<Pystykorva::MatchMode>(args.Value<uint8_t>("mode"));

	options.MinimumSize = args.Value<uint64_t>("minsize");
	options.MaximumSize = args.Value<uint64_t>("maxsize");

	options.MinimumTime = args.Value<std::chrono::file_clock::time_point>("mintime");
	options.MaximumTime = args.Value<std::chrono::file_clock::time_point>("maxtime");

	// On my 16 core CPU, harware_concurrency returns 32, which is fine as I have SMT
	options.MaximumThreads = args.Value<uint32_t>("maxthreads");

	return options;
}

Console& operator << (Console& stream, const Pystykorva::Match& result)
{
	constexpr std::string_view RedColorTagBegin = "\033[31m";
	constexpr std::string_view RedColorTagEnd = "\033[0m";

	stream << result.LineNumber << "\n";

	if (result.Positions.empty())
	{
		return stream;
	}

	std::string line = result.LineContent;
	size_t offset = 0;
	
	for (const auto& position : result.Positions)
	{
		// Red color tag begin
		line.insert(position.Relative.Begin + offset, RedColorTagBegin);
		offset += RedColorTagBegin.size();

		// Color tag end
		line.insert(position.Relative.End + offset, RedColorTagEnd);
		offset += RedColorTagEnd.size();
	}

	// TODO: Trim trailing whitespace, may have a lot

	Cout << line << '\n';

	return stream;
}

std::mutex _mutex;

void ReportProcessing([[maybe_unused]] const std::filesystem::path& path)
{
#if _DEBUG
	_mutex.lock();
	Cout << "Processing: " << path << '\n';
	_mutex.unlock();
#endif
}

size_t Processed = 0;
size_t Skipped = 0;

void ReportResults(const Pystykorva::Result& result)
{
	std::lock_guard<std::mutex> guard(_mutex);

	if (result.StatusMask == Pystykorva::Status::Ok)
	{
		++Processed;
	}
	else
	{
		++Skipped;
	}

#if _DEBUG
	Cout << result.Path << " processed, status: " << Pystykorva::StatusMaskToString(result.StatusMask)
		<< ", matches: " << result.Matches.size() << '\n';
#endif

	for (const Pystykorva::Match& match : result.Matches)
	{
		Cout << result.Path << " @ " << match << '\n';
	}
}

void ReportFinished(std::chrono::milliseconds ms)
{
	std::lock_guard<std::mutex> guard(_mutex);

	Cout << "\nPystykorva finished!\n\n";
	Cout << "Statistics:\n";
	Cout << "\tTook: " << std::format("{:%T}\n", ms);
	Cout << "\tProcessed: " << Processed << " files\n";
	Cout << "\tSkipped: " << Skipped << " files\n";
}

int Run(const std::vector<std::string>& args)
{
	try
	{
		const auto now = std::chrono::file_clock::now();

		const CmdArgs cmdArgs(args,
		{
			{ "help", typeid(std::nullopt), "Prints out this help message" },
			{ "directory", typeid(std::filesystem::path), "The directory to search in" },
			{ "wildcards", typeid(std::set<std::string>), "The file names to match", std::set<std::string>() },
			{ "excludes", typeid(std::set<std::string>), "The directory names to exclude", std::set<std::string>({ ".bzr", ".git", ".hg", ".svn", ".vs" }) },
			{ "searchexpression", typeid(std::u16string), "The text to search" },
			{ "replacement", typeid(std::u16string), "The text to replace", std::u16string() },
			{ "mode", typeid(uint8_t), "0=plain, 1=case insensitive, 2=regex, 3=regex case sensitive", uint8_t(0) },
			{ "minsize", typeid(uint64_t), "Minimum file size", uint64_t(0) },
			{ "maxsize", typeid(uint64_t), "Maximum file size", std::numeric_limits<uint64_t>::max() },
			{ "mintime", typeid(std::chrono::file_clock::time_point), "Minimum file time", now - std::chrono::years(100) },
			{ "maxtime", typeid(std::chrono::file_clock::time_point), "Maximum file time", now + std::chrono::years(100) },
			{ "buffersize", typeid(uint32_t), "Buffer size", uint32_t(0x10000) },
			{ "maxthreads", typeid(uint32_t), "Maximum number of threads", uint32_t(0) }
		});

		Pystykorva::Options options = Deserialize(cmdArgs);
		Pystykorva::Callbacks callbacks;

		callbacks.Started = []()
		{
			Cout << "Pystykorva started!\n";
		};

		callbacks.Processing = ReportProcessing;
		callbacks.Processed = ReportResults;
		callbacks.Finished = ReportFinished;

		Pystykorva pystykorva(options, callbacks);

		pystykorva.Start();
		pystykorva.Wait();
	}
	catch (const CmdArgs::Exception& e)
	{
		Cerr << e.what() << "\n";
		Cerr << e.Usage();
		return 1;
	}
	catch (const std::exception& e)
	{
		Cerr << e.what() << '\n';
		return 2;
	}

	return 0;
}

#ifdef _WIN32

int wmain(int argc, char16_t**)
{
	std::vector<std::string> args;

	for (int i = 0; i < argc; ++i)
	{
		args.emplace_back("TODO: argv to UTF-8");
	}

	return Run(args);
}
#else
int main(int argc, char** argv)
{
	return Run({ argv, argv + argc });
}
#endif