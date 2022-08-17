#include "PCH.hpp"
#include "../PystykorvaLib/Pystykorva.hpp"
#include "Time.hpp"

namespace bpo = boost::program_options;

Pystykorva::Options Parse(const bpo::variables_map& vm)
{
	Pystykorva::Options options;

	options.Directory = vm["directory"].as<std::string>();
	boost::split(options.Wildcards, vm["wildcards"].as<std::string>(), boost::is_any_of("|"));
	boost::split(options.Excludes, vm["excludes"].as<std::string>(), boost::is_any_of("|"));

	options.SearchExpression = vm["searchexpression"].as<std::string>();
	options.ReplacementText = vm["replacement"].as<std::string>();

	options.Mode = vm["mode"].as<Pystykorva::MatchMode>();

	options.MinimumSize = vm["minsize"].as<uint64_t>();
	options.MaximumSize = vm["maxsize"].as<uint64_t>();

	options.MinimumTime = FileTimeFromString(vm["mintime"].as<std::string>());
	options.MaximumTime = FileTimeFromString(vm["maxtime"].as<std::string>());

	return options;
}

int main(int argc, char** argv)
{
	try
	{
		bpo::options_description description("Pystykorva");

		std::string directory;

		description.add_options()
			("help", "Prints out this help message")
			("directory", bpo::value(&directory), "The directory to search in")
			("wildcards", "The file names to match")
			("searchexpression", "The text to search")
			("replacement", "The text to replace")
			("mode", "Plain or regex, case sensitive or not")
			("minsize", "Minimum file size")
			("maxsize", "Maximum file size")
			("mintime", "Minimum file time")
			("maxtime", "Maximum file time");

		bpo::variables_map vm;

		bpo::command_line_parser parser(argc, argv);
		parser.options(description);
		bpo::store(parser.run(), vm);
		bpo::notify(vm);

		if (vm.count("help")) 
		{
			std::cout << description << std::endl;
			return 0;
		}

		Pystykorva::Options options;
		Pystykorva Pystykorva(options);

		Pystykorva.Run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}