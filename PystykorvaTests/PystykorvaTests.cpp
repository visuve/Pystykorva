#include "PCH.hpp"
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
	options.SearchExpression = u"kala";
	options.Mode = Pystykorva::PlainCaseInsensitive;
	// options.MaximumThreads = 1;

	Pystykorva::Callbacks callbacks;

	std::mutex mutex;
	std::map<std::string, Pystykorva::Result> results;

	callbacks.Processed = [&](
		const std::filesystem::path& path,
		const Pystykorva::Result& result)
	{
		mutex.lock(); // Will explode without... ;-D
		results.emplace(path.filename().string(), result);
		mutex.unlock();
	};

	Pystykorva pystykorva(options, callbacks);
	pystykorva.Start();
	pystykorva.Wait();

	ASSERT_EQ(results.size(), 7);

	for (auto& [_, result] : results)
	{
		EXPECT_EQ(result.StatusMask, Pystykorva::Ok);
		EXPECT_EQ(result.Matches.size(), 15);
	}

	// Yes, I know ANSI is a misnomer
	EXPECT_TRUE(results["ANSI.txt"].Encoding.Name.starts_with("ISO-8859"));
	EXPECT_TRUE(results["UTF-8.txt"].Encoding.Name.starts_with("UTF-8"));
	EXPECT_TRUE(results["UTF-8-BOM.txt"].Encoding.Name == "UTF-8");
	EXPECT_TRUE(results["UTF-16BE.txt"].Encoding.Name == "UTF-16BE");
	EXPECT_TRUE(results["UTF-16BE-BOM.txt"].Encoding.Name == "UTF-16BE");
	EXPECT_TRUE(results["UTF-16LE.txt"].Encoding.Name == "UTF-16LE");
	EXPECT_TRUE(results["UTF-16LE-BOM.txt"].Encoding.Name == "UTF-16LE");
}
