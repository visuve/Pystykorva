#include "PCH.hpp"
#include "LineAnalyzer.hpp"

TEST(LineAnalyzerTests, Empty)
{
	LineAnalyzer analyzer;
	std::u16string sample;

	auto result = analyzer.Boundaries(sample);

	ASSERT_TRUE(result.empty());
}

TEST(LineAnalyzerTests, NoBoundary)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 1);
	EXPECT_EQ(result[0].Begin, 0);
	EXPECT_EQ(result[0].End, Pystykorva::Position::Unknown);
}

TEST(LineAnalyzerTests, LF)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\nxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	EXPECT_EQ(result[0].Begin, 0);
	EXPECT_EQ(result[0].End, 4);

	EXPECT_EQ(result[1].Begin, 4);
	EXPECT_EQ(result[1].End, 8);

	EXPECT_EQ(result[2].Begin, 8);
	EXPECT_EQ(result[2].End, Pystykorva::Position::Unknown);
}

TEST(LineAnalyzerTests, CRLF)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\r\nbar\r\nxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	EXPECT_EQ(result[0].Begin, 0);
	EXPECT_EQ(result[0].End, 5);

	EXPECT_EQ(result[1].Begin, 5);
	EXPECT_EQ(result[1].End, 10);

	EXPECT_EQ(result[2].Begin, 10);
	EXPECT_EQ(result[2].End, Pystykorva::Position::Unknown);
}

TEST(LineAnalyzerTests, CR)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\rbar\rxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	EXPECT_EQ(result[0].Begin, 0);
	EXPECT_EQ(result[0].End, 4);

	EXPECT_EQ(result[1].Begin, 4);
	EXPECT_EQ(result[1].End, 8);

	EXPECT_EQ(result[2].Begin, 8);
	EXPECT_EQ(result[2].End, Pystykorva::Position::Unknown);
}

TEST(LineAnalyzerTests, Mixed)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\rxyz\r\nfoobar");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 4);

	EXPECT_EQ(result[0].Begin, 0);
	EXPECT_EQ(result[0].End, 4);

	EXPECT_EQ(result[1].Begin, 4);
	EXPECT_EQ(result[1].End, 8);

	EXPECT_EQ(result[2].Begin, 8);
	EXPECT_EQ(result[2].End, 13);

	EXPECT_EQ(result[3].Begin, 13);
	EXPECT_EQ(result[3].End, Pystykorva::Position::Unknown);
}

TEST(LineAnalyzerTests, Incomplete)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\nbarbababa\0\0\0\0");

	auto result = analyzer.Boundaries(sample);
	
	ASSERT_EQ(result.size(), 3);
	EXPECT_EQ(result[2].Begin, 8);
	EXPECT_EQ(result[2].End, Pystykorva::Position::Unknown);
}