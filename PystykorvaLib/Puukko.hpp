#pragma once

#include "Pystykorva.hpp"

class PuukkoImpl;

class Puukko
{
public:
	Puukko(const Pystykorva::Options& options);
	~Puukko();

	std::map<uint32_t, std::string> Process(const std::filesystem::path&);

private:
	PuukkoImpl* _puukko;
};

