#pragma once

inline std::chrono::time_point<std::chrono::file_clock> 
	FileTimeFromString(const std::string& iso)
{
	std::istringstream iss(iso);

	std::chrono::time_point<std::chrono::file_clock> fc;

	std::chrono::from_stream(iss, "%FT%T", fc);

	return fc;
}