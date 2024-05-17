#include "PystykorvaTests.pch"
#include "UnicodeConverter.hpp"

TEST(UnicodeConverterTests, Simple)
{
	const std::string utf8 = "\xF0\x9F\x98\x98";
	const std::u16string utf16(u"\xD83D\xDE18");

	EXPECT_TRUE(UnicodeConverter::U8toU16(utf8) == utf16);
	EXPECT_TRUE(UnicodeConverter::U16toU8(utf16) == utf8);

	UnicodeConverter converter("UTF-8");
	converter.Convert(utf8, true);

	EXPECT_TRUE(converter.Data() == utf16);
}

TEST(UnicodeConverterTests, Erase)
{
	UnicodeConverter converter("UTF-8");
	converter.Convert("foo\nbar\nxyz", true);

	converter.Erase(4);

	std::u16string expected(u"bar\nxyz");
	EXPECT_TRUE(converter.Data() == expected);
}

TEST(UnicodeConverterTests, View)
{
	UnicodeConverter converter("UTF-8");
	converter.Convert("foo\nbar\nxyz", true);

	std::u16string_view actual = converter.View(4, 8);
	std::u16string expected(u"bar\n");
	EXPECT_TRUE(actual == expected);
}

TEST(UnicodeConverterTests, Trim)
{
	{
		std::u16string text(u"Foobar \xD83D\xDE18");
		UnicodeConverter::RightTrim(text);
		EXPECT_TRUE(text == u"Foobar \xD83D\xDE18");
	}
	{
		std::u16string text(u"Foobar\r\n");
		UnicodeConverter::RightTrim(text);
		EXPECT_TRUE(text == u"Foobar");
	}
	{
		std::u16string text(u"Foobar\t");
		UnicodeConverter::RightTrim(text);
		EXPECT_TRUE(text == u"Foobar");
	}
	{
		std::u16string text(u"Foo bar ");
		UnicodeConverter::RightTrim(text);
		EXPECT_TRUE(text == u"Foo bar");
	}
}

TEST(UnicodeConverterTests, Emoji)
{
	const std::map<std::string, std::string> testData =
	{
		{ "UTF-8", { "\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBF\xE2\x80\x8D\xF0\x9F\xA6\xB2", 15 } },
		{ "UTF-16BE", { "\xD8\x3D\xDC\x68\xD8\x3C\xDF\xFF\x20\x0D\xD8\x3E\xDD\xB2", 14 } },
		{ "UTF-32BE", { "\x00\x01\xF4\x68\x00\x01\xF3\xFF\x00\x00\x20\x0D\x00\x01\xF9\xB2", 16 } }
	};

	for (const auto [encoding, emoji] : testData)
	{
		UnicodeConverter converter(encoding);
		converter.Convert(emoji);
		auto data = converter.Data();
		ASSERT_EQ(data.size(), 7);
		EXPECT_TRUE(data[0] == u'\xD83D');
		EXPECT_TRUE(data[1] == u'\xDC68');
		EXPECT_TRUE(data[2] == u'\xD83C');
		EXPECT_TRUE(data[3] == u'\xDFFF');
		EXPECT_TRUE(data[4] == u'\x200D');
		EXPECT_TRUE(data[5] == u'\xD83E');
		EXPECT_TRUE(data[6] == u'\xDDB2');
	}
}