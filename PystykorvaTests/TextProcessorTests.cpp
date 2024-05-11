#include "PystykorvaTests.pch"
#include "TextProcessor.hpp"
#include "MockFile.hpp"

TEST(TextProcessorTests, RegexSearchUTF8)
{
	std::stop_token token;

	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = u"\\w+";

	TextProcessor processor(token, options);

	MockFile file(u8"\uFEFFAAAA\nBBB\nCC");
	std::vector<Pystykorva::Match> matches;
	Pystykorva::EncodingGuess encoding;
	processor.FindAll(file, matches, encoding);
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-8");
	EXPECT_EQ(encoding.Confidence, 100);

	ASSERT_EQ(matches.size(), 3);

	EXPECT_TRUE(matches[0].LineContent == u"\uFEFFAAAA\n");
	ASSERT_EQ(matches[0].Positions.size(), 1);
	EXPECT_EQ(matches[0].Positions[0], Pystykorva::RelAbsPosPair(1, 5, 1, 5));
	
	EXPECT_TRUE(matches[1].LineContent == u"BBB\n");
	ASSERT_EQ(matches[1].Positions.size(), 1);
	EXPECT_EQ(matches[1].Positions[0], Pystykorva::RelAbsPosPair(0, 3, 6, 9));

	EXPECT_TRUE(matches[2].LineContent == u"CC");
	ASSERT_EQ(matches[2].Positions.size(), 1);
	EXPECT_EQ(matches[2].Positions[0], Pystykorva::RelAbsPosPair(0, 2, 10, 12));
}

TEST(TextProcessorTests, RegexSearchUTF16LE)
{
	std::stop_token token;

	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = u"\\w+";

	TextProcessor processor(token, options);

	// I do not understand why this only works with UTF-16 _BE_ BOM...

	MockFile file(u"\uFEFFAAAA\nBBB\nCC");
	std::vector<Pystykorva::Match> matches;
	Pystykorva::EncodingGuess encoding;
	processor.FindAll(file, matches, encoding);
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-16LE");
	EXPECT_EQ(encoding.Confidence, 100);
	ASSERT_EQ(matches.size(), 3);

	EXPECT_TRUE(matches[0].LineContent == u"\uFEFFAAAA\n");
	ASSERT_EQ(matches[0].Positions.size(), 1);
	EXPECT_EQ(matches[0].Positions[0], Pystykorva::RelAbsPosPair(1, 5, 1, 5));

	EXPECT_TRUE(matches[1].LineContent == u"BBB\n");
	ASSERT_EQ(matches[1].Positions.size(), 1);
	EXPECT_EQ(matches[1].Positions[0], Pystykorva::RelAbsPosPair(0, 3, 12, 15));

	EXPECT_TRUE(matches[2].LineContent == u"CC");
	ASSERT_EQ(matches[2].Positions.size(), 1);
	EXPECT_EQ(matches[2].Positions[0], Pystykorva::RelAbsPosPair(0, 2, 20, 22));
}