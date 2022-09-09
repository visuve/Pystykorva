#include "PCH.hpp"
#include "CmdArgs.hpp"
#include "UnicodeConverter.hpp"

std::ostream& operator << (std::ostream& stream, const CmdArgs::Argument& argument)
{
	stream << std::left << std::setw(25);

	if (argument.Type == typeid(std::nullopt))
	{
		stream << argument.Key;
	}
	else if (argument.Type == typeid(std::filesystem::path))
	{
		stream << argument.Key + "=<file>";
	}
	else if (argument.Type == typeid(double) || argument.Type == typeid(float))
	{
		stream << argument.Key + "=<float>";
	}
	else if (argument.Type == typeid(uint64_t) ||
		argument.Type == typeid(uint32_t) ||
		argument.Type == typeid(uint16_t) ||
		argument.Type == typeid(uint8_t) ||
		argument.Type == typeid(int64_t) ||
		argument.Type == typeid(int32_t) ||
		argument.Type == typeid(int16_t) ||
		argument.Type == typeid(int8_t))
	{
		stream << argument.Key + "=<integer>";
	}
	else if (argument.Type == typeid(std::string) ||
		argument.Type == typeid(std::u16string))
	{
		stream << argument.Key + + "=<text>";
	}
	else if (argument.Type == typeid(std::set<std::string>))
	{
		stream << argument.Key + +"=<set>";
	}
	else if (argument.Type == typeid(std::chrono::file_clock::time_point))
	{
		stream << argument.Key + "=<time>";
	}
	else
	{
		throw std::invalid_argument(std::format("Not supported type: {}", argument.Type.name()));
	}

	return stream << argument.Description;
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
		if (Contains(argument.Key))
		{
			return;
		}
	}
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

CmdArgs::Argument CmdArgs::ExpectedArgumentByKey(std::string_view key) const
{
	const auto keyEquals = [&](const Argument& argument)->bool
	{
		return argument.Key == key;
	};

	auto expectedArgument = std::find_if(_expected.cbegin(), _expected.cend(), keyEquals);

	if (expectedArgument == _expected.cend())
	{
		throw CmdArgs::Exception("Unknown key requested", _usage);
	}

	return *expectedArgument;
}

std::any CmdArgs::ProvidedValueByKey(std::string_view key) const
{
	Argument expected = ExpectedArgumentByKey(key);

	if (expected.Type == typeid(std::nullopt))
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

	if (value.empty())
	{
		if (!expected.DefaultValue.has_value())
		{
			throw CmdArgs::Exception(std::format("Required argument \"{}\" not provided!", key), _usage);
		}

		return expected.DefaultValue.value();
	}

	if (expected.Type == typeid(std::filesystem::path))
	{
		return std::filesystem::path(value);
	}
	else if (expected.Type == typeid(double))
	{
		return std::stod(value);
	}
	else if (expected.Type == typeid(float))
	{
		return std::stof(value);
	}
	else if (expected.Type == typeid(uint64_t))
	{
		return static_cast<uint64_t>(std::stoull(value));
	}
	else if (expected.Type == typeid(uint32_t))
	{
		return static_cast<uint32_t>(std::stoul(value));
	}
	else if (expected.Type == typeid(uint16_t))
	{
		return static_cast<uint16_t>(std::stoul(value));
	}
	else if (expected.Type == typeid(uint8_t))
	{
		return static_cast<uint8_t>(std::stoul(value));
	}
	else if (expected.Type == typeid(int64_t))
	{
		return static_cast<int64_t>(std::stol(value));
	}
	else if (expected.Type == typeid(int32_t))
	{
		return static_cast<int32_t>(std::stoi(value));
	}
	else if (expected.Type == typeid(int16_t))
	{
		return static_cast<int16_t>(std::stoi(value));
	}
	else if (expected.Type == typeid(int8_t))
	{
		return static_cast<int8_t>(std::stoi(value));
	}
	else if (expected.Type == typeid(std::string))
	{
		return value;
	}
	else if (expected.Type == typeid(std::u16string))
	{
		return UnicodeConverter::FromUtf8(value);
	}
	else if (expected.Type == typeid(std::set<std::string>))
	{
		std::set<std::string> result;

		for (std::string_view x : std::views::split(value, ','))
		{
			result.emplace(x);
		}

		return result;
	}
	else if (expected.Type == typeid(std::chrono::file_clock::time_point))
	{
		std::istringstream stream(value);
		std::chrono::file_clock::time_point time;
		std::chrono::from_stream(stream, "%FT%T", time);
		return time;
	}

	throw CmdArgs::Exception("Unsupported argument type requested", _usage);
}