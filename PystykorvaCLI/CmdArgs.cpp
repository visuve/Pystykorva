#include "PCH.hpp"
#include "CmdArgs.hpp"

std::ostream& operator << (std::ostream& stream, const CmdArgs::Argument& argument)
{
	stream << std::left << std::setw(20);

	std::type_index type = std::get<1>(argument);

	if (type == typeid(std::nullopt))
	{
		stream << std::get<0>(argument);
	}
	else if (type == typeid(std::filesystem::path))
	{
		stream << std::get<0>(argument) + "=<file>";
	}
	else if (type == typeid(double) || type == typeid(float))
	{
		stream << std::get<0>(argument) + "=<float>";
	}
	else if (type == typeid(uint64_t) ||
		type == typeid(uint32_t) ||
		type == typeid(uint16_t) ||
		type == typeid(uint8_t) ||
		type == typeid(int64_t) ||
		type == typeid(int32_t) ||
		type == typeid(int16_t) ||
		type == typeid(int8_t))
	{
		stream << std::get<0>(argument) + "=<integer>";
	}
	else if (type == typeid(std::string))
	{
		stream << std::get<0>(argument) + "=<word>";
	}
	else
	{
		throw std::invalid_argument("Not supported type");
	}

	return stream << std::get<2>(argument);
}

CmdArgs::Exception::Exception(
	const std::string& what,
	const std::string& usage) :
	_usage(usage),
	_what("Error: " + what)
{
}

std::string_view CmdArgs::Exception::Usage() const
{
	return _usage;
}

const char* CmdArgs::Exception::what() const throw ()
{
	return _what.c_str();
}

CmdArgs::CmdArgs(const std::vector<std::string>& given, std::initializer_list<Argument> expected) :
	_arguments(given),
	_expected(expected)
{
	std::stringstream usage;

	usage << std::filesystem::path(_arguments[0]).stem().string();
	usage << " - usage:\n\n " << _arguments[0] << std::endl;

	for (const Argument& argument : _expected)
	{
		usage << "  " << argument << std::endl;
	}

	_usage = usage.str();

	for (const Argument& argument : _expected)
	{
		if (Contains(std::get<0>(argument)))
		{
			return;
		}
	}

	throw CmdArgs::Exception("Missing arguments", _usage);
}

CmdArgs::CmdArgs(int argc, char** argv, std::initializer_list<Argument> expected) :
	CmdArgs({ argv, argv + argc }, expected)
{
}

bool CmdArgs::Contains(std::string_view key) const
{
	const auto equals = [&](const std::string& argument)
	{
		return argument == key || argument.starts_with(std::format("{0}=", key));
	};

	return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
}

std::string CmdArgs::Usage() const
{
	return _usage;
}

std::type_index CmdArgs::TypeByKey(std::string_view key) const
{
	const auto keyEquals = [&](const Argument& argument)->bool
	{
		return std::get<0>(argument) == key;
	};

	auto expectedArgument = std::find_if(_expected.cbegin(), _expected.cend(), keyEquals);

	if (expectedArgument == _expected.cend())
	{
		throw CmdArgs::Exception("Unknown key requested", _usage);
	}

	return std::get<1>(*expectedArgument);
}

std::any CmdArgs::ValueByKey(std::string_view key) const
{
	std::type_index type = TypeByKey(key);

	if (type == typeid(std::nullopt))
	{
		if (!Contains(key))
		{
			throw CmdArgs::Exception("Missing value requested", _usage);
		}

		return true;
	}

	std::string value;

	for (std::string_view providedArgument : _arguments)
	{
		if (!providedArgument.starts_with(key))
		{
			continue;
		}
		
		providedArgument.remove_prefix(key.size());

		if (providedArgument.front() != L'=')
		{
			throw CmdArgs::Exception(
				"Arguments with values should be passed with \'=\' sign", _usage);
		}

		providedArgument.remove_prefix(1);
		value = providedArgument;
		break;
	}
	
	if (type == typeid(std::filesystem::path))
	{
		return std::filesystem::path(value);
	}
	else if (type == typeid(double))
	{
		return std::stod(value);
	}
	else if (type == typeid(float))
	{
		return std::stof(value);
	}
	else if (type == typeid(uint64_t))
	{
		return std::stoull(value);
	}
	else if (type == typeid(uint32_t))
	{
		return std::stoul(value);
	}
	else if (type == typeid(uint16_t))
	{
		return static_cast<uint16_t>(std::stoul(value));
	}
	else if (type == typeid(uint8_t))
	{
		return static_cast<uint8_t>(std::stoul(value));
	}
	else if (type == typeid(int64_t))
	{
		return std::stol(value);
	}
	else if (type == typeid(int32_t))
	{
		return std::stoi(value);
	}
	else if (type == typeid(int16_t))
	{
		return  static_cast<uint16_t>(std::stoi(value));
	}
	else if (type == typeid(int8_t))
	{
		return  static_cast<uint8_t>(std::stoi(value));
	}
	else if (type == typeid(std::string))
	{
		return value;
	}
	else if (type == typeid(std::set<std::string>))
	{
		std::set<std::string> result;

		for (std::string_view x : std::views::split(value, '|'))
		{
			result.emplace(x);
		}

		return result;
	}
	else if (type == typeid(std::chrono::file_clock::time_point))
	{
		std::istringstream stream(value);
		std::chrono::file_clock::time_point time;
		std::chrono::from_stream(stream, "%FT%T", time);
		return time;
	}

	throw CmdArgs::Exception("Unsupported argument type requested", _usage);
}