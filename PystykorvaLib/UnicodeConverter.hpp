#pragma once

class UnicodeConverterImpl;

struct ConversionException : std::runtime_error
{
	inline ConversionException(const std::string& message) :
		std::runtime_error(message)
	{
	}
};

class UnicodeConverter
{
public:
	UnicodeConverter(std::string_view encoding);
	~UnicodeConverter();

	void Convert(std::string_view sample, bool flush);
	std::u16string_view Data() const;
	size_t End() const;
	std::u16string_view View(size_t from, size_t to) const;

	void Erase(size_t index);

private:
	UnicodeConverterImpl* _impl;
};

