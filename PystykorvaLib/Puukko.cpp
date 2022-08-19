#include "PCH.hpp"
#include "Puukko.hpp"

class PuukkoImpl
{
public:
	PuukkoImpl(const Pystykorva::Options& options)
	{

	}

	std::map<uint32_t, std::string> Process(const std::filesystem::path& path)
	{
		return {};
	}
};

Puukko::Puukko(const Pystykorva::Options& options) :
	_puukko(new PuukkoImpl(options))
{
}

Puukko::~Puukko()
{
	delete _puukko;
}

std::map<uint32_t, std::string> Puukko::Process(const std::filesystem::path& path)
{
	return _puukko->Process(path);
}
