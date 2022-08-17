#include "PCH.hpp"

#include "../PystykorvaCLI/Time.hpp"

BOOST_AUTO_TEST_CASE(FileTimeFromStringTest)
{
	std::chrono::time_point<std::chrono::file_clock> timestamp;

	BOOST_CHECK_NO_THROW(timestamp = FileTimeFromString("2022-08-14T06:59:27"));

	auto compileTime = std::filesystem::last_write_time(__FILE__);

	BOOST_CHECK(compileTime > timestamp);

	BOOST_CHECK_NO_THROW(timestamp = FileTimeFromString("Haha!"));

	BOOST_CHECK(timestamp.time_since_epoch() == 
		std::chrono::system_clock::from_time_t(0).time_since_epoch());
}