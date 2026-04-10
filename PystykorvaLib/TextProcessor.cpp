#include "PystykorvaLib.pch"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

TextProcessor::TextProcessor(std::stop_token token, const Pystykorva::Options& options) :
	_token(token),
	_options(options)
{
}

TextProcessor::~TextProcessor()
{
}

Pystykorva::Result TextProcessor::ProcessPath(const std::filesystem::path& path)
{
	Pystykorva::Result result;
	result.Path = path;

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

		uint64_t fileSize = std::filesystem::file_size(path);

		if (fileSize == 0 || fileSize < _options.MinimumSize)
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

		std::fstream input(path);

		ProcessFile(input, result.Matches);
	}
	/*catch (const IOException&)
	{
		result.StatusMask |= Pystykorva::Status::IOError;
	}
	catch (const EncodingException&)
	{
		result.StatusMask |= Pystykorva::Status::EncodingError;
	}
	catch (const ConversionException&)
	{
		result.StatusMask |= Pystykorva::Status::ConversionError;
	}
	catch (const SearchException&)
	{
		result.StatusMask |= Pystykorva::Status::SearchError;
	}*/
	catch (const std::exception&)
	{
		result.StatusMask |= Pystykorva::Status::UnknownError;
	}

	return result;
}

void TextProcessor::ProcessFile(std::fstream&, std::vector<Pystykorva::Match>&)
{
	// TODO
}