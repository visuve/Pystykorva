#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "CmdArgs.hpp"
#include "Console.hpp"
#include "UnicodeConverter.hpp"

Console Cout(Console::StandardOutput);
Console Cerr(Console::StandardError);

Pystykorva::Options Deserialize(const CmdArgs& args)
{
	Pystykorva::Options options;

	options.Directory = args.Value<std::filesystem::path>("directory");
	options.IncludeWildcards = args.Value<std::set<std::string>>("wildcards");
	options.ExcludedDirectories = args.Value<std::set<std::string>>("excludes");

	options.SearchExpression = args.Value<std::u16string>("searchexpression");
	options.ReplacementText = args.Value<std::u16string>("replacement");

	options.Mode = static_cast<Pystykorva::MatchMode>(args.Value<uint8_t>("mode"));

	options.MinimumSize = args.Value<uint64_t>("minsize");
	options.MaximumSize = args.Value<uint64_t>("maxsize");

	options.MinimumTime = args.Value<std::chrono::file_clock::time_point>("mintime");
	options.MaximumTime = args.Value<std::chrono::file_clock::time_point>("maxtime");

	// 64 kib should be decent for most text files
	options.BufferSize = args.Value<uint32_t>("buffersize");

	// On my 16 core CPU, harware_concurrency returns 32, which is fine as I have SMT
	options.MaximumThreads = args.Value<uint32_t>("maxthreads");

	return options;
}

std::string StatusMaskToString(uint32_t statusMask)
{
	if (!statusMask)
	{
		return "ok";
	}

	std::string result;
	std::string delimiter;

	for (uint32_t bit = 1; statusMask; statusMask &= ~bit, bit <<= 1)
	{
		switch (statusMask & bit)
		{
			case Pystykorva::Status::Missing:
				result += delimiter + "the file is missing";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NoPermission:
				result += delimiter + "incorrect permissions";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NameExcluded:
				result += delimiter + "excluded by name";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooSmall:
				result += delimiter + "too small file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooBig:
				result += delimiter + "too large file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooEarly:
				result += delimiter + "file cretead too early";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooLate:
				result += delimiter + "file cretead too late";
				delimiter = ", ";
				break;
			case Pystykorva::Status::EncodingError:
				result += delimiter + "probably a binary file";
				delimiter = ", ";
				break;
			case Pystykorva::Status::ConversionError:
				result += delimiter + "unicode conversion error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::IOError:
				result += delimiter + "i/o error";
				delimiter = ", ";
				break;
		}
	}

	return result;
}

Console& operator << (Console& stream, const Pystykorva::Match& result)
{
	constexpr static std::u16string_view RedColorTagBegin = u"\033[31m";
	constexpr static std::u16string_view RedColorTagEnd = u"\033[0m";

	stream << result.LineNumber << "\n";

	if (result.LinePositions.empty())
	{
		return stream;
	}

	std::u16string line = result.LineContent;
	size_t offset = 0;
	
	for (const auto& position : result.LinePositions)
	{
		// Red color tag begin
		line.insert(position.Begin + offset, RedColorTagBegin);
		offset += RedColorTagBegin.size();

		// Color tag end
		line.insert(position.End + offset, RedColorTagEnd);
		offset += RedColorTagEnd.size();
	}

	// Trim trailing whitespace, may have a lot
	UnicodeConverter::RightTrim(line);

	Cout << line << '\n';

	return stream;
}

std::mutex _mutex;

void ReportProcessing(
	const std::filesystem::path& path)
{
	_CRT_UNUSED(path);
#if _DEBUG
	_mutex.lock();
	Cout << "Processing: " << path << '\n';
	_mutex.unlock();
#endif
}

void ReportResults(
	const std::filesystem::path& path, 
	const Pystykorva::Result& result)
{
	std::lock_guard<std::mutex> guard(_mutex);

#if _DEBUG
	Cout << path << " processed, status: " << StatusMaskToString(result.StatusMask)
		<< ", encoding: " << result.Encoding.Name << ", confidence: " << result.Encoding.Confidence 
		<< ", matches: " << result.Matches.size() << '\n';
#endif

	for (const Pystykorva::Match& match : result.Matches)
	{
		Cout << path << " @ " << match << '\n';
	}
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

		callbacks.Finished = [](std::chrono::milliseconds ms)
		{
			Cout << "Pystykorva finished!\n";
			Cout << "Took: " << ms << '\n';
		};

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

int wmain(int argc, char16_t** argv)
{
	std::vector<std::string> args;

	for (int i = 0; i < argc; ++i)
	{
		args.emplace_back(UnicodeConverter::U16toU8(argv[i]));
	}

	return Run(args);
}
#else
int main(int argc, char** argv)
{
	return Run({ argv, argv + argc });
}
#endif