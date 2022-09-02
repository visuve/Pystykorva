#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "CmdArgs.hpp"

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

	// 64 kib should be decent for most text files
	options.BufferSize = args.Value<uint32_t>("buffersize");

	// On my 16 core CPU, harware_concurrency returns 32, which is fine as I have SMT
	options.MaximumThreads = args.Value<uint32_t>("maxthreads");

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
			case Pystykorva::Status::ConversionError:
				std::cout << delimiter << "Unicode conversion error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::IOError:
				std::cout << delimiter << "I/O error";
				delimiter = ", ";
				break;
		}
	}

	std::cout << '\n';
}

std::ostream& operator << (std::ostream& stream, const Pystykorva::Match& result)
{
	stream << result.LineNumber << "\n";

	if (result.Positions.empty())
	{
		return stream;
	}

	std::u16string line = result.Content;
	size_t offset = 0;
	
	for (const auto& position : result.Positions)
	{
		// Red color tag begin
		line.insert(position.Start + offset, u"\033[31m");
		offset += 5;

		// Color tag end
		line.insert(position.End + offset, u"\033[0m");
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
	int32_t length = 0;

	u_strToUTF8(
		nullptr,
		0,
		&length,
		line.data(),
		static_cast<int32_t>(line.size()),
		&status);

	assert(status == U_BUFFER_OVERFLOW_ERROR);

	status = U_ZERO_ERROR;

	std::string buffer(static_cast<size_t>(length), 0);

	u_strToUTF8(
		buffer.data(),
		length,
		&length,
		line.data(),
		static_cast<int32_t>(line.size()),
		&status);

	assert(status == U_STRING_NOT_TERMINATED_WARNING);

	std::cout << buffer;
#endif

	return stream;
}


std::mutex _mutex;

void ReportResults(std::filesystem::path path, Pystykorva::Result result)
{
	std::lock_guard<std::mutex> guard(_mutex);

	// wcout to display paths correctly in Windows
	std::wcout << path;
	std::cout << " processed, status: ";

	PrintStatusMask(result.StatusMask);

	for (const Pystykorva::Match& result : result.Matches)
	{
		std::wcout << path;
		std::cout << " @ " << result << std::endl;
	}
}

int main(int argc, char** argv)
{
	try
	{
		const auto now = std::chrono::file_clock::now();

		const CmdArgs cmdArgs(argc, argv,
		{
			{ "help", typeid(std::nullopt), "Prints out this help message" },
			{ "directory", typeid(std::filesystem::path), "The directory to search in" },
			{ "wildcards", typeid(std::string), "The file names to match", std::set<std::string>({ "*" }) },
			{ "excludes", typeid(std::string), "The directory names to exclude", std::set<std::string>({ ".bzr", ".git", ".hg", ".svn", ".vs" }) },
			{ "searchexpression", typeid(std::string), "The text to search" },
			{ "replacement", typeid(std::string), "The text to replace", std::string() },
			{ "mode", typeid(uint8_t), "Plain or regex, case sensitive or not", uint8_t(1)},
			{ "minsize", typeid(uint64_t), "Minimum file size", uint64_t(0) },
			{ "maxsize", typeid(uint64_t), "Maximum file size", std::numeric_limits<uint64_t>::max() },
			{ "mintime", typeid(std::chrono::file_clock::time_point), "Minimum file time", now - std::chrono::years(100) },
			{ "maxtime", typeid(std::chrono::file_clock::time_point), "Maximum file time", now + std::chrono::years(100) },
			{ "buffersize", typeid(uint32_t), "Buffer size", 0x10000u },
			{ "maxthreads", typeid(uint32_t), "Maximum number of threads", std::thread::hardware_concurrency() }
		});

		Pystykorva::Options options = Deserialize(cmdArgs);
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
	catch (const CmdArgs::Exception& e)
	{
		std::cerr << e.what() << "\n";
		std::cerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}