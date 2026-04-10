#include "PystykorvaLib.pch"
#include "Pystykorva.hpp"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

Pystykorva::Pystykorva(const Options& options, const Callbacks& callbacks) :
	_options(options),
	_callbacks(callbacks),
	_rdi(options.Directory)
{
}

Pystykorva::~Pystykorva()
{
	Stop();
}

void Pystykorva::Start()
{
	if (!_options.MaximumThreads)
	{
		_options.MaximumThreads = 1;
	}

	_start = std::chrono::high_resolution_clock::now();

	if (_callbacks.Started)
	{
		_callbacks.Started();
	}

	_threads.reserve(_options.MaximumThreads);

	const auto worker = std::bind_front(&Pystykorva::Worker, this);

	for (uint32_t i = 0; i < _options.MaximumThreads; ++i)
	{
		_threads.emplace_back(worker, _stopSource.get_token());
	}
}

void Pystykorva::Wait()
{
	for (std::jthread& thread : _threads)
	{
		thread.join();
	}
}

void Pystykorva::Stop()
{
	_stopSource.request_stop();
	_threads.clear();
}

std::string Pystykorva::StatusMaskToString(uint32_t statusMask)
{
	if (!statusMask)
	{
		return "ok";
	}

	std::string result;
	std::string delimiter;

	for (uint32_t bit = 1; statusMask; statusMask &= ~bit, bit <<= 1)
	{
		switch (statusMask & bit)
		{
			case Pystykorva::Status::Missing:
				result += delimiter + "the file is missing";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NoPermission:
				result += delimiter + "incorrect permissions";
				delimiter = ", ";
				break;
			case Pystykorva::Status::NameExcluded:
				result += delimiter + "excluded by name";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooSmall:
				result += delimiter + "too small file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooBig:
				result += delimiter + "too large file size";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooEarly:
				result += delimiter + "file cretead too early";
				delimiter = ", ";
				break;
			case Pystykorva::Status::TooLate:
				result += delimiter + "file cretead too late";
				delimiter = ", ";
				break;
			case Pystykorva::Status::IOError:
				result += delimiter + "i/o error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::EncodingError:
				result += delimiter + "encoding error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::ConversionError:
				result += delimiter + "conversion error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::SearchError:
				result += delimiter + "line search error";
				delimiter = ", ";
				break;
			case Pystykorva::Status::UnknownError:
				result += delimiter + "unknown error";
				delimiter = ", ";
				break;
		}
	}

	return result;
}
bool Pystykorva::IsExcludedDirectory(const std::filesystem::path& path) const
{
	return std::any_of(
		_options.ExcludedDirectories.cbegin(),
		_options.ExcludedDirectories.cend(),
		[&](std::string_view excluded)
	{
		return path.stem() == excluded;
	});
}

std::filesystem::path Pystykorva::Next()
{
	std::lock_guard<std::mutex> lock(_mutex);

	for (;_rdi != std::filesystem::recursive_directory_iterator(); ++_rdi)
	{
		const std::filesystem::path path = _rdi->path();

		if (_rdi->is_regular_file())
		{
			++_rdi; // Prepare iterator for the next caller
			return path;
		}
		
		if (_rdi->is_directory() && IsExcludedDirectory(path))
		{
			_rdi.disable_recursion_pending();
		}
	}

	return {};
}

void Pystykorva::Worker(std::stop_token token)
{
	TextProcessor textProcessor(token, _options);

	while (!token.stop_requested())
	{
		std::filesystem::path path = Next();

		if (path.empty())
		{
			break;
		}

		if (_callbacks.Processing)
		{
			_callbacks.Processing(path);
		}

		Result result = textProcessor.ProcessPath(path);

		if (_callbacks.Processed)
		{
			_callbacks.Processed(result);
		}
	}
}
