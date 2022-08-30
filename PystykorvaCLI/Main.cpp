#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "CmdArgs.hpp"

Pystykorva::Options Parse(const CmdArgs& args)
{
	Pystykorva::Options options;

	options.Directory = args.Value<std::filesystem::path>("directory");
	options.IncludeWildcards = args.Value<std::set<std::string>>("wildcards", {});
	options.ExcludedDirectories = args.Value<std::set<std::string>>("excludes", { ".bzr", ".git", ".hg", ".svn", ".vs" });

	options.SearchExpression = args.Value<std::string>("searchexpression");
	options.ReplacementText = args.Value<std::string>("replacement", "");

	options.Mode = static_cast<Pystykorva::MatchMode>(args.Value<uint8_t>("mode"), Pystykorva::PlainCaseInsensitive);

	options.MinimumSize = args.Value<uint64_t>("minsize", 0);
	options.MaximumSize = args.Value<uint64_t>("maxsize", std::numeric_limits<uint64_t>::max());

	auto now = std::chrono::file_clock::now();

	// I think two milleania is wide enough scale for defaults...
	options.MinimumTime = args.Value<std::chrono::file_clock::time_point>("mintime", now - std::chrono::years(1000));
	options.MaximumTime = args.Value<std::chrono::file_clock::time_point>("maxtime", now + std::chrono::years(1000));

	// 64 kib should be decent for most text files
	options.BufferSize = args.Value<uint32_t>("buffersize", 0x10000);

	// On my 16 core CPU, harware_concurrency returns 32, which is fine as I have SMT
	options.MaximumThreads = args.Value<uint32_t>("maxthreads", std::thread::hardware_concurrency());

	return options;
}

void PrintStatusMask(uint32_t statusMask)
{
	if (!statusMask)
	{
		std::cout << "ok\n";
	}

	std::string delimiter;

	for (uint32_t bit = 1; statusMask; statusMask &= ~bit, bit <<= 1)
	{
		switch (statusMask & bit)
		{
			case Pystykorva::Status::Missing:
				std::cout << delimiter << "the file is missing";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NoPermission:
				std::cout << delimiter << "incorrect permissions";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NameExcluded:
				std::cout << delimiter << "excluded by name";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooSmall:
				std::cout << delimiter << "too small file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooBig:
				std::cout << delimiter << "too large file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooEarly:
				std::cout << delimiter << "file cretead too early";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooLate:
				std::cout << delimiter << "file cretead too late";
				delimiter = ", ";
				break;
			case Pystykorva::Status::UnknownEncoding:
				std::cout << delimiter << "probably a binary file";
				delimiter = ", ";
				break;
		}
	}

	std::cout << '\n';
}

std::ostream& operator << (std::ostream& stream, const Pystykorva::Result& result)
{
	stream << result.LineNumber << "\n";

	if (result.Matches.empty())
	{
		return stream;
	}

	std::u16string line = result.Content;
	size_t offset = 0;
	
	for (const auto& match : result.Matches)
	{
		// Red color tag begin
		line.insert(match.Start + offset, u"\033[31m");
		offset += 5;

		// Color tag end
		line.insert(match.End + offset, u"\033[0m");
		offset += 4;
	}

#ifdef _WIN32
	static HANDLE out(GetStdHandle(STD_OUTPUT_HANDLE));
	DWORD written = 0;

	if (!WriteConsoleW(out, line.data(), static_cast<DWORD>(line.size()), &written, nullptr))
	{
		std::unreachable();
	}

	assert(written > 0);
#else
	UErrorCode status = U_ZERO_ERROR;
	int32_t len = 0;

	u_strToUTF8(
		nullptr,
		0,
		&len,
		line.data(),
		line.size(),
		&status);

	assert(status == U_BUFFER_OVERFLOW_ERROR);

	std::string buffer(len, 0);

	u_strToUTF8(
		buffer.data(),
		len,
		&len,
		line.data(),
		line.size(),
		&status);

	assert(status == U_ZERO_ERROR);

	std::cout << buffer;
#endif

	return stream;
}


std::mutex _mutex;

void ReportResults(std::filesystem::path path, uint32_t mask, std::vector<Pystykorva::Result> results)
{
	std::lock_guard<std::mutex> guard(_mutex);

	std::wcout << L"Processed: " << path << L" status: ";

	PrintStatusMask(mask);

	for (const Pystykorva::Result& result : results)
	{
		std::wcout << path;
		std::cout << " @ " << result << std::endl;
	}
}

int main(int argc, char** argv)
{
	try
	{
		CmdArgs cmdArgs(argc, argv,
		{
			{ "help", typeid(std::nullopt), "Prints out this help message" },
			{ "directory", typeid(std::filesystem::path), "The directory to search in" },
			{ "wildcards", typeid(std::string), "The file names to match" },
			{ "excludes", typeid(std::string), "The directory names to exclude" },
			{ "searchexpression", typeid(std::string), "The text to search" },
			{ "replacement", typeid(std::string), "The text to replace" },
			{ "mode", typeid(uint8_t), "Plain or regex, case sensitive or not" },
			{ "minsize", typeid(uint64_t), "Minimum file size" },
			{ "maxsize", typeid(uint64_t), "Maximum file size" },
			{ "mintime", typeid(std::chrono::file_clock::time_point), "Minimum file time" },
			{ "maxtime", typeid(std::chrono::file_clock::time_point), "Maximum file time" },
			{ "buffersize", typeid(uint32_t), "Buffer size threads" },
			{ "maxthreads", typeid(uint32_t), "Maximum number of threads" },
		});

		Pystykorva::Options options = Parse(cmdArgs);
		Pystykorva::Callbacks callbacks;

		std::ios::sync_with_stdio(false);

		callbacks.Started = []()
		{
			std::cout << "Pystykorva started!\n";
		};

		callbacks.Processing = [](std::filesystem::path path)
		{
			_mutex.lock();
			std::cout << "Processing: " << path << '\n';
			_mutex.unlock();
		};

		callbacks.Processed = ReportResults;

		callbacks.Finished = [](std::chrono::milliseconds ms)
		{
			std::cout << "Pystykorva started!\n";
			std::cout << "Took: " << ms << std::endl;
		};

		Pystykorva pystykorva(options, callbacks);

		pystykorva.Start();
		pystykorva.Wait();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}