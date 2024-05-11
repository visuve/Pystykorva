#include "PystykorvaTests.pch"
#include "TextReplacer.hpp"
#include "MockFile.hpp"

TEST(TextReplacedTests, ReplaceSameSize)
{
	MockFile input(u8"foo\nbar\nxyz");
	MockFile output(11);

	Pystykorva::Result result;
	result.Encoding = { 100, "UTF-8" };
	std::vector<Pystykorva::RelAbsPosPair> foo = { { 0, 3, 0, 3 } };
	std::vector<Pystykorva::RelAbsPosPair> bar = { { 0, 3, 4, 7 } };
	std::vector<Pystykorva::RelAbsPosPair> xyz = { { 0, 3, 8, 11 } };
	result.Matches.emplace_back(0, u"foo\n", foo);
	result.Matches.emplace_back(1, u"bar\n", bar);
	result.Matches.emplace_back(2, u"xyz\n", xyz);

	TextReplacer replacer(input, result);
	replacer.ReplaceAll(output, u"abc");

	EXPECT_TRUE(output.Data() == "abc\nabc\nabc");
	EXPECT_TRUE(result.Matches[0].LineContent == u"abc\n");
	EXPECT_TRUE(result.Matches[1].LineContent == u"abc\n");
	EXPECT_TRUE(result.Matches[2].LineContent == u"abc\n");
}

TEST(TextReplacedTests, ReplaceSmallerSize)
{
	MockFile input(u8"foo\nbar\nxyz");
	MockFile output(8);

	Pystykorva::Result result;
	result.Encoding = { 100, "UTF-8" };
	std::vector<Pystykorva::RelAbsPosPair> foo = { { 0, 3, 0, 3 } };
	std::vector<Pystykorva::RelAbsPosPair> bar = { { 0, 3, 4, 7 } };
	std::vector<Pystykorva::RelAbsPosPair> xyz = { { 0, 3, 8, 11 } };
	result.Matches.emplace_back(0, u"foo\n", foo);
	result.Matches.emplace_back(1, u"bar\n", bar);
	result.Matches.emplace_back(2, u"xyz\n", xyz);

	TextReplacer replacer(input, result);
	replacer.ReplaceAll(output, u"ab");

	EXPECT_TRUE(output.Data() == "ab\nab\nab");
	EXPECT_TRUE(result.Matches[0].LineContent == u"ab\n");
	EXPECT_TRUE(result.Matches[1].LineContent == u"ab\n");
	EXPECT_TRUE(result.Matches[2].LineContent == u"ab\n");
}

TEST(TextReplacedTests, ReplaceLargerSize)
{
	MockFile input(u8"foo\nbar\nxyz");
	MockFile output(14);

	Pystykorva::Result result;
	result.Encoding = { 100, "UTF-8" };
	std::vector<Pystykorva::RelAbsPosPair> foo = { { 0, 3, 0, 3 } };
	std::vector<Pystykorva::RelAbsPosPair> bar = { { 0, 3, 4, 7 } };
	std::vector<Pystykorva::RelAbsPosPair> xyz = { { 0, 3, 8, 11 } };
	result.Matches.emplace_back(0, u"foo\n", foo);
	result.Matches.emplace_back(1, u"bar\n", bar);
	result.Matches.emplace_back(2, u"xyz\n", xyz);

	TextReplacer replacer(input, result);
	replacer.ReplaceAll(output, u"abcd");

	EXPECT_TRUE(output.Data() == "abcd\nabcd\nabcd");
	EXPECT_TRUE(result.Matches[0].LineContent == u"abcd\n");
	EXPECT_TRUE(result.Matches[1].LineContent == u"abcd\n");
	EXPECT_TRUE(result.Matches[2].LineContent == u"abcd\n");
}