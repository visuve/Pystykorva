#include "PCH.hpp"
#include "Puukko.hpp"
#include "UnicodeConverter.hpp"

Puukko::Puukko(const Pystykorva::Options& options, std::stop_token token) :
	_options(options),
	_token(token),
	_textSearcher(u"TODO!", options.Mode)
{
}

Puukko::~Puukko()
{
}

std::map<uint32_t, Puukko::Result> Puukko::ProcessFile(const std::filesystem::path& path)
{
	const uintmax_t fileSize = std::filesystem::file_size(path);
	std::string buffer(std::min(uintmax_t(_options.BufferSize), fileSize), 0);

	std::fstream::openmode mode = std::fstream::in | std::fstream::binary;

	if (!_options.ReplacementText.empty())
	{
		mode |= std::fstream::out;
	}

	std::fstream file(path, mode);
	std::unique_ptr<UnicodeConverter> converter;

	size_t line = 0;

	while (!_token.stop_requested() && file)
	{
		file.read(buffer.data(), buffer.size());

		const size_t bytesRead = static_cast<size_t>(file.gcount());

		if (bytesRead < buffer.size())
		{
			buffer.resize(bytesRead);
		}

		// TODO: detect binary garble

		if (!converter)
		{
			std::string encoding = _encodingDetector.DetectEncoding(buffer);
			converter = std::make_unique<UnicodeConverter>(encoding);
		}

		converter->Convert(buffer, file.eof());

		auto boundaries = _lineAnalyzer.Boundaries(converter->Data());

		for (const LineAnalyzer::LineBoundary& boundary : boundaries)
		{
			if (!boundary.End.has_value())
			{
				break;
			}

			++line;

			std::u16string_view view = 
				converter->View(boundary.Begin, boundary.End.value());

			ProcessLine(view);
		}

		if (!boundaries.empty())
		{
			converter->Erase(boundaries.back().Begin);
		}
	}

	return {};
}

std::vector<TextSearcher::Result> Puukko::ProcessLine(std::u16string_view& line)
{
	// TODO: call searcher
	return {};
}
