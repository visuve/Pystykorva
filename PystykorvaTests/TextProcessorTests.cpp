#include "PystykorvaTests.pch"
#include "TextProcessor.hpp"

class FakeFile : public Pystykorva::IFile
{
public:
	template<typename T, size_t N>
	FakeFile(T(&data)[N])
	{
		_size = sizeof(T) * (N - 1); // Exclude the trailing nulls
		_data = static_cast<char*>(malloc(_size));

		if (_data)
		{
			memcpy(_data, data, _size);
		}
	}

	~FakeFile()
	{
		if (_data)
		{
			free(_data);
		}
	}

	std::string_view Sample(size_t size = 0x400) const override
	{
		return { _data, std::min(size, _size) };
	}

	std::string_view Data() const override
	{
		return { _data, _size };
	}
private:
	char* _data = nullptr;
	size_t _size = 0;
};

TEST(TextProcessorTests, RegexSearchUTF8)
{
	std::stop_token token;

	Pystykorva::Options options = {};
	options.Mode = Pystykorva::MatchMode::RegexCaseInsensitive;
	options.SearchExpression = u"\\w+";

	TextProcessor processor(token, options);

	FakeFile file(u8"\uFEFFAAAA\nBBB\nCC");
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

	FakeFile file(u"\uFEFFAAAA\nBBB\nCC");
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