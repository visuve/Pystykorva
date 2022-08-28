#include "PCH.hpp"
#include "Wildcard.hpp"

TEST(WildCardTests, NoWildcard)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "foobar"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "foobar"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "FOOBAR"));

	EXPECT_FALSE(Wildcard::Matches("xoobar", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("XOOBAR", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("xoobar", "FOOBAR"));

	EXPECT_FALSE(Wildcard::Matches("zooxar", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("ZOOXAR", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("zooxar", "FOOBAR"));

	EXPECT_FALSE(Wildcard::Matches("foobax", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("FOOBAX", "foobar"));
	EXPECT_FALSE(Wildcard::Matches("foobax", "FOOBAR"));

	EXPECT_FALSE(Wildcard::Matches("foobar", "xoobar"));
	EXPECT_FALSE(Wildcard::Matches("FOOBAR", "xoobar"));
	EXPECT_FALSE(Wildcard::Matches("foobar", "XOOBAR"));

	EXPECT_FALSE(Wildcard::Matches("foobar", "fooxar"));
	EXPECT_FALSE(Wildcard::Matches("FOOBAR", "fooxar"));
	EXPECT_FALSE(Wildcard::Matches("foobar", "FOOXAR"));

	EXPECT_FALSE(Wildcard::Matches("foobar", "foobax"));
	EXPECT_FALSE(Wildcard::Matches("FOOBAR", "foobax"));
	EXPECT_FALSE(Wildcard::Matches("foobar", "FOOBAX"));
}

TEST(WildCardTests, AsteriskStart)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "*bar"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "*bar"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "*BAR"));
}

TEST(WildCardTests, AsteriskMiddle)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "f*r"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "f*r"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "F*R"));
}

TEST(WildCardTests, AsteriskEnd)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "foo*"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "foo*"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "FOO*"));
}
TEST(WildCardTests, AsteriskMulti)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "f*b*r"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "f*b*r"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "F*B*R"));
}

TEST(WildCardTests, QuestionStart)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "?oobar"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "?oobar"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "?OOBAR"));
}

TEST(WildCardTests, QuestionMiddle)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "foo?ar"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "foo?ar"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "FOO*AR"));
}

TEST(WildCardTests, QuestionEnd)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "fooba?"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "fooba?"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "FOOBA?"));
}

TEST(WildCardTests, QuestionMulti)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "?oo?a?"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "?oo?a?"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "?OO?A?"));
}

TEST(WildCardTests, MixedMulti)
{
	EXPECT_TRUE(Wildcard::Matches("foobar", "f*b?r"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "f*b?r"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "f*B?r"));

	EXPECT_TRUE(Wildcard::Matches("foobar", "f??b*r"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "f??b*r"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "f??B*r"));

	EXPECT_TRUE(Wildcard::Matches("foobar", "?*?a?"));
	EXPECT_TRUE(Wildcard::Matches("FOOBAR", "?*?a?"));
	EXPECT_TRUE(Wildcard::Matches("foobar", "?*?A?"));
}