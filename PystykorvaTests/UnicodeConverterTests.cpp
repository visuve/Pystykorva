#include "PCH.hpp"
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