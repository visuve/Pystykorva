#include "PCH.hpp"
#include "../PystykorvaLib/EncodingDetector.hpp"

TEST(EncodingDetectorTests, UTF8BOM)
{
	Pystykorva::EncodingGuess encoding;
	EXPECT_TRUE(EncodingDetector().DetectEncoding("\xEF\xBB\xBF\xF0\x9F\x98\x98", encoding));
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-8");
	EXPECT_EQ(encoding.Confidence, 100);
}

TEST(EncodingDetectorTests, UTF16BEBOM)
{
	Pystykorva::EncodingGuess encoding;
	EXPECT_TRUE(EncodingDetector().DetectEncoding("\xFE\xFF\xD8\x3D\xDE\x18", encoding));
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-16BE");
	EXPECT_EQ(encoding.Confidence, 100);
}

TEST(EncodingDetectorTests, UTF16LEBOM)
{
	Pystykorva::EncodingGuess encoding;
	EXPECT_TRUE(EncodingDetector().DetectEncoding("\xFF\xFE\x3D\xD8\x18\xDE", encoding));
	EXPECT_STREQ(encoding.Name.c_str(), "UTF-16LE");
	EXPECT_EQ(encoding.Confidence, 100);
}