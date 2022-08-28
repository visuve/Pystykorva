#include "PCH.hpp"
#include "UnicodeConverter.hpp"

TEST(UnicodeConverterTests, Simple)
{
	UnicodeConverter converter("UTF-8");
	converter.Convert("\xF0\x9F\x98\x98", true);

	std::u16string expected({ 0xD83D, 0xDE18 });
	EXPECT_TRUE(converter.Data() == expected);
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