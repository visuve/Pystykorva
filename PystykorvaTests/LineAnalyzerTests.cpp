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
	ASSERT_EQ(result[0].Begin, 0);
	ASSERT_EQ(result[0].End, Pystykorva::FilePosition::Unknown);
}

TEST(LineAnalyzerTests, LF)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\nxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	ASSERT_EQ(result[0].Begin, 0);
	ASSERT_EQ(result[0].End, 4);

	ASSERT_EQ(result[1].Begin, 4);
	ASSERT_EQ(result[1].End, 8);

	ASSERT_EQ(result[2].Begin, 8);
	ASSERT_EQ(result[2].End, Pystykorva::FilePosition::Unknown);
}

TEST(LineAnalyzerTests, CRLF)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\r\nbar\r\nxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	ASSERT_EQ(result[0].Begin, 0);
	ASSERT_EQ(result[0].End, 5);

	ASSERT_EQ(result[1].Begin, 5);
	ASSERT_EQ(result[1].End, 10);

	ASSERT_EQ(result[2].Begin, 10);
	ASSERT_EQ(result[2].End, Pystykorva::FilePosition::Unknown);
}

TEST(LineAnalyzerTests, CR)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\rbar\rxyz");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 3);

	ASSERT_EQ(result[0].Begin, 0);
	ASSERT_EQ(result[0].End, 4);

	ASSERT_EQ(result[1].Begin, 4);
	ASSERT_EQ(result[1].End, 8);

	ASSERT_EQ(result[2].Begin, 8);
	ASSERT_EQ(result[2].End, Pystykorva::FilePosition::Unknown);
}

TEST(LineAnalyzerTests, Mixed)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\rxyz\r\nfoobar");

	auto result = analyzer.Boundaries(sample);

	ASSERT_EQ(result.size(), 4);

	ASSERT_EQ(result[0].Begin, 0);
	ASSERT_EQ(result[0].End, 4);

	ASSERT_EQ(result[1].Begin, 4);
	ASSERT_EQ(result[1].End, 8);

	ASSERT_EQ(result[2].Begin, 8);
	ASSERT_EQ(result[2].End, 13);

	ASSERT_EQ(result[3].Begin, 13);
	ASSERT_EQ(result[3].End, Pystykorva::FilePosition::Unknown);
}

TEST(LineAnalyzerTests, Incomplete)
{
	LineAnalyzer analyzer;
	std::u16string sample(u"foo\nbar\nbarbababa\0\0\0\0");

	auto result = analyzer.Boundaries(sample);
	
	ASSERT_EQ(result.size(), 3);
	ASSERT_EQ(result[2].Begin, 8);
	ASSERT_EQ(result[2].End, Pystykorva::FilePosition::Unknown);
}