#include "PCH.hpp"
#include "TextSearcher.hpp"

TEST(TextSearcherTests, PlainSearch)
{
	{
		TextSearcher searcher(u"bar", Pystykorva::PlainCaseSensitive);

		auto result = searcher.FindIn(u"foo bar xyz");

		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0].Start, 4);
		EXPECT_EQ(result[0].End, 7);
	}
	{
		TextSearcher searcher(u"BAR", Pystykorva::PlainCaseSensitive);

		auto result = searcher.FindIn(u"foo bar xyz");

		EXPECT_TRUE(result.empty());
	}
	{
		TextSearcher searcher(u"bar", Pystykorva::PlainCaseInsensitive);

		auto result = searcher.FindIn(u"foo bar xyz");

		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0].Start, 4);
		EXPECT_EQ(result[0].End, 7);
	}
	{
		TextSearcher searcher(u"BAR", Pystykorva::PlainCaseInsensitive);

		auto result = searcher.FindIn(u"foo bar xyz");

		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0].Start, 4);
		EXPECT_EQ(result[0].End, 7);
	}
}

TEST(TextSearcherTests, RegexSearch)
{
	TextSearcher searcher(u"\\w+", Pystykorva::RegexCaseSensitive);
	auto result = searcher.FindIn(u"foo bar xyz");
	ASSERT_EQ(result.size(), 3);

	EXPECT_EQ(result[0].Start, 0);
	EXPECT_EQ(result[0].End, 3);

	EXPECT_EQ(result[1].Start, 4);
	EXPECT_EQ(result[1].End, 7);

	EXPECT_EQ(result[2].Start, 8);
	EXPECT_EQ(result[2].End, 11);
}