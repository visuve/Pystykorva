#include "PCH.hpp"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

TextProcessor::TextProcessor(std::stop_token token, const Pystykorva::Options& options) :
	_token(token),
	_options(options),
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
			std::bind(Wildcard::Matches, path.filename().string(), std::placeholders::_1)))
		{
			result.StatusMask |= Pystykorva::Status::NameExcluded;
		}

		// TODO: check file permission

		uintmax_t fileSize = std::filesystem::file_size(path);

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

		std::ifstream file(path, std::ios::binary);
		BufferedStream stream(file, _options.BufferSize, fileSize);

		FindAll(stream, result.Matches, result.Encoding);
	}
	catch (const EncodingException&)
	{
		result.StatusMask |= Pystykorva::Status::EncodingError;
	}
	catch (const ConversionException&)
	{
		result.StatusMask |= Pystykorva::Status::ConversionError;
	}
	catch (const AnalysisException&)
	{
		result.StatusMask |= Pystykorva::Status::AnalysisError;
	}
	catch (const SearchException&)
	{
		result.StatusMask |= Pystykorva::Status::SearchError;
	}
	catch (const std::exception&)
	{
		result.StatusMask |= Pystykorva::Status::IOError;
	}

	return result;
}

void TextProcessor::FindAll(BufferedStream& stream, std::vector<Pystykorva::Match>& matches, Pystykorva::EncodingGuess& encoding)
{
	std::unique_ptr<UnicodeConverter> converter;

	uint32_t lineNumber = 0;

	uint64_t offset = 0;
	uint8_t charSize = 0;

	while (!_token.stop_requested())
	{
		if (!stream.Read())
		{
			return;
		}

		if (!converter)
		{
			if (!_encodingDetector.DetectEncoding(stream.Data(), encoding))
			{
				throw EncodingException("Failed to detect encoding");
			}

			converter = std::make_unique<UnicodeConverter>(encoding.Name);
			charSize = converter->CharSize();
		}

		// NOTE: the converter's back buffer might grow larger than defined in the options
		converter->Convert(stream.Data(), stream.HasData() == false);

		auto boundaries = _lineAnalyzer.Boundaries(converter->Data());

		for (Pystykorva::Position& boundary : boundaries)
		{
			// Check if the boundary is "incomplete"
			if (boundary.End == Pystykorva::Position::Unknown)
			{
				if (stream.HasData())
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
				converter->View(boundary.Begin, boundary.End);

			Pystykorva::Match match = ProcessLine(offset, lineNumber, line);

			// I smell problems here with variable-length character encoding...
			offset += line.size() * charSize;

			if (match.Positions.empty())
			{
				continue;
			}

			matches.emplace_back(match);
		}

		if (!boundaries.empty() && stream.HasData())
		{
			// Erase data which has already been analyzed
			// Note: there is no point in erasing the data
			// if the stream is already at the end
			converter->Erase(boundaries.back().Begin);
		}
	}
}

Pystykorva::Match TextProcessor::ProcessLine(uint64_t offset, uint32_t lineNumber, std::u16string_view line)
{
	Pystykorva::Match result;
	result.LineNumber = lineNumber;
	result.LineContent = line;

	for (const Pystykorva::Position& position : _textSearcher.FindIn(line))
	{
		result.Positions.emplace_back(position, offset);
	}

	return result;
}
