#include "PCH.hpp"
#include "Pystykorva.hpp"

void FindAll(
	const std::filesystem::path& path,
	const Pystykorva::Options& options,
	const Pystykorva::Callbacks& callbacks)
{

}

void ReplaceAll(
	const std::filesystem::path& path,
	const Pystykorva::Options& options,
	const Pystykorva::Callbacks& callbacks)
{

}


Pystykorva::Pystykorva(const Options& options, const Callbacks& callbacks) :
	_options(options),
	_callbacks(callbacks)
{
	unsigned threads =
		_options.MaximumThreads == 0 ?
		std::thread::hardware_concurrency() * 2 :
		_options.MaximumThreads;

	_pool = std::make_unique<boost::asio::thread_pool>(threads);
}

Pystykorva::~Pystykorva()
{
	Stop();
}

void Pystykorva::Start()
{
	auto start = std::chrono::high_resolution_clock::now();

	_run = true;

	if (_callbacks.Started)
	{
		_callbacks.Started();
	}


	for (const auto& directoryEntry : std::filesystem::recursive_directory_iterator(_options.Directory))
	{
		if (!directoryEntry.is_regular_file())
		{
			continue;
		}

		if (_options.ReplacementText.empty())
		{
			boost::asio::post(*_pool, [=]()
			{
				FindAll(directoryEntry.path(), _options, _callbacks);
			});
		}
		else
		{
			boost::asio::post(*_pool, [=]()
			{
				ReplaceAll(directoryEntry.path(), _options, _callbacks);
			});
		}
	}

	_pool->join();
	
	if (_callbacks.Finished)
	{
		auto diff = start - std::chrono::high_resolution_clock::now();
		_callbacks.Finished(std::chrono::duration_cast<std::chrono::milliseconds>(diff));
	}
}

void Pystykorva::Stop()
{
	_run = false;
	_pool->join();
}