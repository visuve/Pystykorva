#include "PCH.hpp"
#include "BufferedStream.hpp"

TEST(StreamTests, ReadSimple)
{
	{
		std::istringstream iss("foobar");
		BufferedStream stream(iss, 3, 6);

		EXPECT_TRUE(stream.Read());
		EXPECT_TRUE(stream.Data() == "foo");
		EXPECT_TRUE(stream.Read());
		EXPECT_TRUE(stream.Data() == "bar");
		EXPECT_FALSE(stream.HasData());
		EXPECT_FALSE(stream.Read());
	}
	{
		std::istringstream iss("foobar");
		BufferedStream stream(iss, 4, 6);

		EXPECT_TRUE(stream.Read());
		EXPECT_TRUE(stream.Data() == "foob");
		EXPECT_TRUE(stream.Read());
		EXPECT_TRUE(stream.Data() == "ar");
		EXPECT_FALSE(stream.HasData());
		EXPECT_FALSE(stream.Read());
	}
}