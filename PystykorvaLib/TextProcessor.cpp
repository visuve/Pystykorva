#include "PCH.hpp"
#include "TextProcessor.hpp"

std::streamsize StreamSize(std::iostream& stream)
{
	stream.seekg(0, std::ios::end);
	std::streampos end = stream.tellg();

	stream.seekg(0, std::ios::beg);
	std::streampos beg = stream.tellg();

	return end - beg;
}

TextProcessor::TextProcessor(const Pystykorva::Options& options, std::stop_token token) :
	_options(options),
	_token(token),
	_textSearcher(options.SearchExpression, options.Mode)
{
}

TextProcessor::~TextProcessor()
{
}

std::vector<Pystykorva::Result> TextProcessor::ProcessFile(const std::filesystem::path& path)
{
	std::fstream::openmode mode;

	if (_options.ReplacementText.empty())
	{
		mode = std::ios::in | std::ios::binary;
	}
	else
	{
		mode = std::ios::in | std::ios::out | std::ios::binary;
	}

	std::fstream file(path, mode);

	return ProcessStream(file);
}

std::vector<Pystykorva::Result> TextProcessor::ProcessStream(std::iostream& stream)
{
	std::streamsize streamSize = StreamSize(stream);
	std::streamsize bufferSize = std::min(std::streamsize(_options.BufferSize), streamSize);
	std::string buffer(static_cast<size_t>(bufferSize), 0);

	std::unique_ptr<UnicodeConverter> converter;

	std::vector<Pystykorva::Result> results;
	uint32_t lineNumber = 0;

	while (!_token.stop_requested() && stream)
	{
		stream.read(buffer.data(), buffer.size());

		const size_t bytesRead = static_cast<size_t>(stream.gcount());

		if (bytesRead == 0)
		{
			// This seems to happen sometimes
			break;
		}

		if (bytesRead < buffer.size())
		{
			// This should happen only once, i.e. when the last chunk is read
			buffer.resize(bytesRead);
		}

		if (!converter)
		{
			std::string encoding = _encodingDetector.DetectEncoding(buffer);

			if (encoding == "Binary")
			{
				break;
			}

			converter = std::make_unique<UnicodeConverter>(encoding);
		}

		// NOTE: the converter's back buffer might grow larger than defined in the options
		converter->Convert(buffer, stream.eof());

		auto boundaries = _lineAnalyzer.Boundaries(converter->Data());

		for (LineAnalyzer::LineBoundary& boundary : boundaries)
		{
			// Check if the boundary is "incomplete"
			if (!boundary.End.has_value())
			{
				if (stream)
				{
					// Fetch more data by breaking out of the loop
					break;
				}
				else
				{
					// There is no more data:
					// force the boundary end to the data
					boundary.End = converter->End();
				}
			}

			// Do not increment on "incomplete" lines
			++lineNumber;

			std::u16string_view line =
				converter->View(boundary.Begin, boundary.End.value());

			Pystykorva::Result result = ProcessLine(line, lineNumber);

			if (result.Matches.empty())
			{
				continue;
			}

			results.emplace_back(result);
		}

		if (!boundaries.empty() && stream)
		{
			// Erase data which has already been analyzed
			// Note: there is no point in erasing the data
			// if the stream is already at the end
			converter->Erase(boundaries.back().Begin);
		}
	}

	return results;
}

Pystykorva::Result TextProcessor::ProcessLine(std::u16string_view& line, uint32_t number)
{
	Pystykorva::Result result;
	result.LineNumber = number;
	result.Content = line;
	result.Matches = _textSearcher.FindIn(line);
	return result;
}