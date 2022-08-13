#include "PCH.hpp"
#include "../BoostGrepLib/BoostGrep.hpp"

namespace bpo = boost::program_options;

int main(int argc, char** argv)
{
	try
	{
		bpo::options_description description("BoostGrep");

		std::string directory;

		description.add_options()
			("help", "Prints out this help message")
			("directory", bpo::value(&directory), "The directory to search in")
			("wildcards", "The file names to match")
			("searchword", "The string to search");

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

		BoostGrep x;
		x.SayHello();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}