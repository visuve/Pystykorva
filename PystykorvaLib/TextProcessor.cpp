#include "PCH.hpp"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

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

Pystykorva::Result TextProcessor::ProcessFile(const std::filesystem::path& path)
{
	Pystykorva::Result result;

	try
	{
		if (!std::filesystem::exists(path))
		{
			result.StatusMask |= Pystykorva::Status::Missing;
			return result;
		}

		if (!_options.IncludeWildcards.empty() && std::none_of(
			_options.IncludeWildcards.cbegin(),
			_options.IncludeWildcards.cend(),
			std::bind(Wildcard::Matches, path.string(), std::placeholders::_1)))
		{
			result.StatusMask |= Pystykorva::Status::NameExcluded;
		}

		// TODO: check file permission

		auto fileSize = std::filesystem::file_size(path);

		if (fileSize < _options.MinimumSize || fileSize == 0)
		{
			result.StatusMask |= Pystykorva::Status::TooSmall;
		}
		else if (fileSize > _options.MaximumSize)
		{
			result.StatusMask |= Pystykorva::Status::TooBig;
		}

		auto writeTime = std::filesystem::last_write_time(path);

		if (writeTime < _options.MinimumTime)
		{
			result.StatusMask |= Pystykorva::Status::TooEarly;
		}
		else if (writeTime > _options.MaximumTime)
		{
			result.StatusMask |= Pystykorva::Status::TooLate;
		}

		if (result.StatusMask != 0)
		{
			return result;
		}

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

		ProcessStream(result.Matches, file, fileSize);

	}
	catch (const EncodingException&)
	{
		result.StatusMask |= Pystykorva::Status::UnknownEncoding;
	}
	catch (const ConversionException&)
	{
		result.StatusMask |= Pystykorva::Status::ConversionError;
	}
	catch (const std::exception&)
	{
		result.StatusMask |= Pystykorva::Status::IOError;
	}

	return result;
}

void TextProcessor::ProcessStream(std::vector<Pystykorva::Match>& matches, std::iostream& stream, std::streamsize size)
{
	if (size <= 0)
	{
		size = StreamSize(stream);
	}
	
	assert(size > 0);

	std::streamsize bufferSize = std::min(std::streamsize(_options.BufferSize), size);
	std::string buffer(static_cast<size_t>(bufferSize), 0);

	std::unique_ptr<UnicodeConverter> converter;

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
			const std::string encoding = _encodingDetector.DetectEncoding(buffer);
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

			Pystykorva::Match match = ProcessLine(line, lineNumber);

			if (match.Positions.empty())
			{
				continue;
			}

			matches.emplace_back(match);
		}

		if (!boundaries.empty() && stream)
		{
			// Erase data which has already been analyzed
			// Note: there is no point in erasing the data
			// if the stream is already at the end
			converter->Erase(boundaries.back().Begin);
		}
	}
}

Pystykorva::Match TextProcessor::ProcessLine(std::u16string_view line, uint32_t number)
{
	Pystykorva::Match result;
	result.LineNumber = number;
	result.Content = line;
	result.Positions = _textSearcher.FindIn(line);
	return result;
}
