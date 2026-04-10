#include "PystykorvaTests.pch"
#include "Pystykorva.hpp"

std::filesystem::path TestDataPath()
{
	auto current = std::filesystem::current_path();

	while (current.stem() != "Pystykorva")
	{
		current = current.parent_path();

		if (!current.has_stem())
		{
			throw std::runtime_error("Test data not found");
		}
	}

	return current / "PystykorvaTests" / "Data";
}

TEST(PystykorvaTests, SearchSuccess)
{
	Pystykorva::Options options;

	options.Directory = TestDataPath();
	options.IncludeWildcards = { "*.txt" };
	options.SearchExpression = "kala";
	options.Mode = Pystykorva::PlainCaseInsensitive;
	// options.MaximumThreads = 1;

	Pystykorva::Callbacks callbacks;

	std::mutex mutex;
	std::map<std::string, Pystykorva::Result> results;

	callbacks.Processed = [&](
		const Pystykorva::Result& result)
	{
		mutex.lock(); // Will explode without... ;-D
		results.emplace(result.Path.filename().string(), result);
		mutex.unlock();
	};

	Pystykorva pystykorva(options, callbacks);
	pystykorva.Start();
	pystykorva.Wait();

	ASSERT_EQ(results.size(), 7);

	for (auto& [_, result] : results)
	{
		EXPECT_EQ(result.Matches.size(), 15);
	}
}
