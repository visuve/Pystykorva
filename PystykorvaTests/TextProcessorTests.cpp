#include "PCH.hpp"
#include "TextProcessor.hpp"

TEST(TextProcessorTests, RegexSearch)
{
	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = u"\\w+";
	options.BufferSize = 3;

	std::stop_token token;

	TextProcessor processor(options, token);

	std::istringstream iss("foo\nbar\nxyz");

	BufferedStream stream(iss, 3, 11);
	std::vector<Pystykorva::Match> matches;
	Pystykorva::EncodingGuess encoding;
	processor.FindAll(stream, matches, encoding);

	ASSERT_EQ(matches.size(), 3);
	EXPECT_TRUE(matches[0].LineContent == u"foo\n");
	EXPECT_TRUE(matches[1].LineContent == u"bar\n");
	EXPECT_TRUE(matches[2].LineContent == u"xyz");
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-8");
	EXPECT_EQ(encoding.Confidence, 15);
}