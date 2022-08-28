#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "TextProcessor.hpp"
#include "Wildcard.hpp"

Pystykorva::Pystykorva(const Options& options, const Callbacks& callbacks) :
	_options(options),
	_callbacks(callbacks),
	_rdi(options.Directory)
{
	assert(_options.BufferSize > _options.SearchExpression.size());
}

Pystykorva::~Pystykorva()
{
	Stop();
}

void Pystykorva::Start()
{
	_start = std::chrono::high_resolution_clock::now();

	if (_callbacks.Started)
	{
		_callbacks.Started();
	}

	assert(_options.MaximumThreads >= 1);

	for (uint32_t i = 0; i < _options.MaximumThreads; ++i)
	{
		_threads.emplace_back(std::bind_front(&Pystykorva::Worker, this));
	}
}

void Pystykorva::Wait()
{
	for (std::jthread& thread : _threads)
	{
		thread.join();
	}

	if (_callbacks.Finished)
	{
		auto diff = std::chrono::high_resolution_clock::now() - _start;
		_callbacks.Finished(std::chrono::duration_cast<std::chrono::milliseconds>(diff));
	}
}

void Pystykorva::Stop()
{
	_threads.clear();
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

	std::filesystem::path result;

	while (_rdi != std::filesystem::recursive_directory_iterator())
	{
		const std::filesystem::path path = _rdi->path();

		assert(!path.empty());

		if (_rdi->is_regular_file())
		{
			result = path;
		}
		else if (_rdi->is_directory() && IsExcludedDirectory(path))
		{
			_rdi.disable_recursion_pending();
		}

		++_rdi;

		if (!result.empty())
		{
			break;
		}
	}

	return result;
}

uint32_t Pystykorva::FileStatus(const std::filesystem::path& path)
{
	uint32_t status = Status::Ok;

	// TODO: check file permission

	if (!std::filesystem::exists(path))
	{
		return Status::Missing;
	}

	if (!_options.IncludeWildcards.empty() && std::none_of(
		_options.IncludeWildcards.cbegin(),
		_options.IncludeWildcards.cbegin(),
		std::bind(Wildcard::Matches, path.string(), std::placeholders::_1)))
	{
		return Status::NameExcluded;
	}

	auto fileSize = std::filesystem::file_size(path);

	if (fileSize < _options.MinimumSize || fileSize == 0)
	{
		status |= Status::TooSmall;
	}

	if (fileSize > _options.MaximumSize)
	{
		status |= Status::TooBig;
	}

	auto writeTime = std::filesystem::last_write_time(path);

	if (writeTime < _options.MinimumTime)
	{
		status |= Status::TooEarly;
	}

	if (writeTime > _options.MaximumTime)
	{
		status |= Status::TooLate;
	}

	return status;
}

void Pystykorva::Worker(std::stop_token token)
{
	TextProcessor textProcessor(_options, token);

	while (!token.stop_requested())
	{
		std::filesystem::path path = Next();

		if (path.empty())
		{
			continue;
		}

		if (_callbacks.Processing)
		{
			_callbacks.Processing(path);
		}

		uint32_t status = FileStatus(path);
		std::vector<Pystykorva::Result> results;

		if (status == 0)
		{
			results = textProcessor.ProcessFile(path);
		}

		if (_callbacks.Processed)
		{
			_callbacks.Processed(path, results, status);
		}
	}
}
