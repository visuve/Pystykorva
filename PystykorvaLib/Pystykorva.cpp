#include "PCH.hpp"
#include "Pystykorva.hpp"
#include "Puukko.hpp"

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
	auto start = std::chrono::high_resolution_clock::now();

	if (_callbacks.Started)
	{
		_callbacks.Started();
	}

	uint32_t maxThreads = _options.MaximumThreads == 0 ?
		std::thread::hardware_concurrency() : _options.MaximumThreads;

	for (uint32_t i = 0; i < maxThreads; ++i)
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
}

void Pystykorva::Stop()
{
	_threads.clear();
}

std::filesystem::path Pystykorva::Next()
{
	std::lock_guard<std::mutex> lock(_mutex);

	std::error_code ec;

	do 
	{
		_rdi.increment(ec);

		if (_rdi->is_regular_file())
		{
			return _rdi->path();
		}

	} while (ec.value() != 0);

	return {};
}

uint32_t Pystykorva::FileStatus(const std::filesystem::path& path)
{
	uint32_t status = Status::Ok;

	if (!std::filesystem::exists(path))
	{
		return Status::Missing;
	}

	auto fileSize = std::filesystem::file_size(path);

	if (fileSize > _options.MinimumSize)
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
	Puukko puukko(_options);

	while (!token.stop_requested())
	{
		std::filesystem::path path = Next();

		if (path.empty())
		{
			return;
		}

		if (_callbacks.Processing)
		{
			_callbacks.Processing(path);
		}

		uint32_t status = FileStatus(path);
		std::map<uint32_t, std::string> results;

		if (status != 0)
		{
			puukko.Process(path);
		}

		if (_callbacks.Processed)
		{
			_callbacks.Processed(path, results, status);
		}
	}
}
