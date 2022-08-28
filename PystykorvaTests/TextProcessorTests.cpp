#include "PCH.hpp"
#include "TextProcessor.hpp"

TEST(TextProcessorTests, RegexSearch)
{
	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = "\\w+";
	options.BufferSize = 3;

	std::stop_token token;

	TextProcessor processor(options, token);

	std::stringstream stream;
	stream << "foo\nbar\nxyz";

	auto results = processor.ProcessStream(stream);

	EXPECT_EQ(results.size(), 3);
	EXPECT_TRUE(results[1].Content == u"foo\n");
	EXPECT_TRUE(results[2].Content == u"bar\n");
	EXPECT_TRUE(results[3].Content == u"xyz");
}