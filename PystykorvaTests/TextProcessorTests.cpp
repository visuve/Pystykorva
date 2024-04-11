#include "PCH.hpp"
#include "TextProcessor.hpp"

TEST(TextProcessorTests, RegexSearchUTF8)
{
	std::stop_token token;

	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = u"\\w+";
	options.BufferSize = 3;

	TextProcessor processor(token, options);

	constexpr char8_t src[] = u8"\uFEFFAAAA\nBBB\nCC"; // I do not understand why this only works with UTF-16 _BE_ BOM...
	constexpr size_t srcSize = sizeof(src) - 1; // Exclude the trailing null

	char dst[srcSize] = {};
	memcpy(dst, src, srcSize);

	std::istringstream iss({ reinterpret_cast<const char*>(src), srcSize });

	//	0	1	2	3	4	5	6	7	8	9	10	11
	//	x	A	A	A	A	\n	B	B	B	\n	C	C

	BufferedStream stream(iss, 3, srcSize);
	std::vector<Pystykorva::Match> matches;
	Pystykorva::EncodingGuess encoding;
	processor.FindAll(stream, matches, encoding);
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
	options.BufferSize = 3;

	TextProcessor processor(token, options);

	constexpr char16_t src[] = u"\uFEFFAAAA\nBBB\nCC"; // I do not understand why this only works with UTF-16 _BE_ BOM...
	constexpr size_t srcSize = sizeof(src) - 2; // Exclude the trailing null

	char dst[srcSize] = {};
	memcpy(dst, src, srcSize);

	std::istringstream iss({ reinterpret_cast<const char*>(src), srcSize });

	// It is simply impossible to use buffer sizes, which are not divisible by two when the source data is unicode
	BufferedStream stream(iss, 3, srcSize);
	std::vector<Pystykorva::Match> matches;
	Pystykorva::EncodingGuess encoding;
	processor.FindAll(stream, matches, encoding);
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