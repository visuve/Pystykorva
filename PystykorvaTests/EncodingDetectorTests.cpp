#include "PCH.hpp"
#include "../PystykorvaLib/EncodingDetector.hpp"

TEST(EncodingDetectorTests, UTF8BOM)
{
	EXPECT_STREQ(EncodingDetector().DetectEncoding("\xEF\xBB\xBF\xF0\x9F\x98\x98").c_str(), "UTF-8");
}

TEST(EncodingDetectorTests, UTF16BEBOM)
{
	EXPECT_STREQ(EncodingDetector().DetectEncoding("\xFE\xFF\xD8\x3D\xDE\x18").c_str(), "UTF-16BE");
}

TEST(EncodingDetectorTests, UTF16LEBOM)
{
	EXPECT_STREQ(EncodingDetector().DetectEncoding("\xFF\xFE\x3D\xD8\x18\xDE").c_str(), "UTF-16LE");
}