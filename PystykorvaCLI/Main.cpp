#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "CmdArgs.hpp"

Pystykorva::Options Parse(const CmdArgs& args)
{
	Pystykorva::Options options;

	options.Directory = args.Value<std::filesystem::path>("directory");
	options.IncludeWildcards = args.Value<std::set<std::string>>("wildcards", {});
	options.ExcludedDirectories = args.Value<std::set<std::string>>("excludes", { ".bzr", ".git", ".hg", ".svn" });

	options.SearchExpression = args.Value<std::string>("searchexpression");
	options.ReplacementText = args.Value<std::string>("replacement", "");

	options.Mode = static_cast<Pystykorva::MatchMode>(args.Value<uint8_t>("mode"), Pystykorva::PlainCaseSensitive);

	options.MinimumSize = args.Value<uint64_t>("minsize", 0);
	options.MaximumSize = args.Value<uint64_t>("maxsize", std::numeric_limits<uint64_t>::max());

	auto now = std::chrono::file_clock::now();

	// I think two milleania is wide enough scale for defaults...
	options.MinimumTime = args.Value<std::chrono::file_clock::time_point>("mintime", now - std::chrono::years(1000));
	options.MaximumTime = args.Value<std::chrono::file_clock::time_point>("maxtime", now + std::chrono::years(1000));

	// Mebibyte should be decent for any operation
	options.BufferSize = args.Value<uint32_t>("buffersize", 0x100000);

	// On my 16 core CPU, harware_concurrency returns 32, which is fine as I have SMT
	options.MaximumThreads = args.Value<uint32_t>("maxthreads", std::thread::hardware_concurrency());

	return options;
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

		callbacks.Started = []()
		{
			std::cout << "Pystykorva started!" << std::endl;
		};

		//callbacks.Processing = [](std::filesystem::path path)
		//{
		//	std::cout << "Processing: " << path << std::endl;
		//};

		callbacks.Processed = [](std::filesystem::path path, 
			std::vector<Pystykorva::Result> results, 
			uint32_t)
		{
			// std::cout << "Processed: " << path << " status mask = " << statusMask << std::endl;

			for (const Pystykorva::Result& result : results)
			{
				std::cout << path << ": " << result.LineNumber << " " << std::endl;
			}
		};

		callbacks.Finished = [](std::chrono::milliseconds ms)
		{
			std::cout << "Pystykorva started!" << std::endl;
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