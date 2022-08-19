#include "PCH.hpp"
#include "../PystykorvaLib/Pystykorva.hpp"
#include "CmdArgs.hpp"

Pystykorva::Options Parse(const CmdArgs& args)
{
	Pystykorva::Options options;

	options.Directory = args.Value<std::string>("directory");
	options.Wildcards = args.Value<std::set<std::string>>("wildcards");
	options.Excludes = args.Value<std::set<std::string>>("excludes");

	options.SearchExpression = args.Value<std::string>("searchexpression");
	options.ReplacementText = args.Value<std::string>("replacement");

	options.Mode = static_cast<Pystykorva::MatchMode>(args.Value<uint8_t>("mode"));

	options.MinimumSize = args.Value<uint64_t>("minsize");
	options.MaximumSize = args.Value<uint64_t>("maxsize");

	options.MinimumTime = args.Value<std::chrono::file_clock::time_point>("mintime");
	options.MaximumTime = args.Value<std::chrono::file_clock::time_point>("maxtime");

	options.MaximumThreads = args.Value<uint32_t>("maxthreads");

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
			{ "wildcards", typeid(std::string), "The file names to match"},
			{ "searchexpression", typeid(std::string), "The text to search"},
			{ "replacement", typeid(std::string), "The text to replace"},
			{ "mode", typeid(int), "Plain or regex, case sensitive or not"},
			{ "minsize", typeid(int), "Minimum file size"},
			{ "maxsize", typeid(int), "Maximum file size"},
			{ "mintime", typeid(std::chrono::system_clock::time_point), "Minimum file time"},
			{ "maxtime", typeid(std::chrono::system_clock::time_point), "Maximum file time"},
			{ "maxthreads", typeid(int), "Maximum number of threads"}
		});

		Pystykorva::Options options = Parse(cmdArgs);
		Pystykorva::Callbacks callbacks;
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