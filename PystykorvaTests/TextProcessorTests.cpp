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

	std::vector<Pystykorva::Match> matches;
	BufferedStream stream(iss, 3, 11);

	processor.ProcessStream(matches, stream);

	ASSERT_EQ(matches.size(), 3);
	EXPECT_TRUE(matches[0].Content == u"foo\n");
	EXPECT_TRUE(matches[1].Content == u"bar\n");
	EXPECT_TRUE(matches[2].Content == u"xyz");
}