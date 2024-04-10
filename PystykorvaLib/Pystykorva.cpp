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

	if (!_options.MaximumThreads)
	{
		uint32_t maxConcurrentThreads = std::thread::hardware_concurrency();
		_options.MaximumThreads = maxConcurrentThreads ? maxConcurrentThreads : 1;
	}

	const auto worker = std::bind_front(&Pystykorva::Worker, this);

	for (uint32_t i = 0; i < _options.MaximumThreads; ++i)
	{
		_threads.emplace_back(worker);
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
	TextProcessor textProcessor(_options, token);

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

		Result result = textProcessor.ProcessFile(path);

		if (_callbacks.Processed)
		{
			_callbacks.Processed(path, result);
		}
	}
}
