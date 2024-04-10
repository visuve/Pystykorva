#include "PCH.hpp"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

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

		ProcessStream(result.Matches, stream);

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

void TextProcessor::ProcessStream(std::vector<Pystykorva::Match>& matches, BufferedStream& stream)
{
	std::unique_ptr<UnicodeConverter> converter;

	uint32_t lineNumber = 0;

	while (!_token.stop_requested() && stream.Read())
	{
		if (!converter)
		{
			const std::string encoding = _encodingDetector.DetectEncoding(stream.Data());
			converter = std::make_unique<UnicodeConverter>(encoding);
		}

		// NOTE: the converter's back buffer might grow larger than defined in the options
		converter->Convert(stream.Data(), stream.HasData() == false);

		auto boundaries = _lineAnalyzer.Boundaries(converter->Data());

		for (Pystykorva::FilePosition& boundary : boundaries)
		{
			// Check if the boundary is "incomplete"
			if (boundary.End == Pystykorva::FilePosition::Unknown)
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

			Pystykorva::Match match = ProcessLine(stream.Offset(), lineNumber, line);

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
	auto positions = _textSearcher.FindIn(line);

	for (auto& position : positions)
	{
		position.Begin += offset;
		position.End += offset;
	}

	Pystykorva::Match result;
	result.LineNumber = lineNumber;
	result.Positions = positions;
	result.Content = line;
	return result;
}
