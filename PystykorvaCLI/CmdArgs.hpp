#pragma once

class CmdArgs
{
public:
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& what, const std::string& usage);
		std::string_view Usage() const;
		const char* what() const throw ();

	private:
		const std::string _usage;
		const std::string _what;
	};

	struct Argument
	{
		inline Argument(std::string&& key, std::type_index&& type, std::string&& description) :
			Key(key),
			Type(type),
			Description(description)
		{
		}

		inline Argument(std::string&& key, std::type_index&& type, std::string&& description, std::any&& defaultValue) :
			Key(key),
			Type(type),
			Description(description),
			DefaultValue(defaultValue)
		{
		}

		std::string Key;
		std::type_index Type;
		std::string Description;
		std::optional<std::any> DefaultValue;
	};

	CmdArgs(const std::vector<std::string>& given, std::initializer_list<Argument> expected);
	
	CmdArgs(const CmdArgs&) = delete;
	CmdArgs(CmdArgs&&) = delete;

	CmdArgs& operator = (const CmdArgs&) = delete;
	CmdArgs& operator = (CmdArgs&&) = delete;

	bool Contains(std::string_view key) const;

	template<typename T>
	T Value(std::string_view key) const
	{
		return std::any_cast<T>(ProvidedValueByKey(key));
	}

private:
	Argument ExpectedArgumentByKey(std::string_view key) const;
	std::any ProvidedValueByKey(std::string_view key) const;

	std::string _usage;
	const std::vector<Argument> _expected;
	const std::vector<std::string> _arguments;
};