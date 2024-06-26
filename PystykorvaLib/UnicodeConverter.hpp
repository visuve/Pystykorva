#pragma once

#include "NonCopyable.hpp"

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
	NonCopyable(UnicodeConverter);

	uint8_t CharSize() const;
	void Convert(std::string_view sample, bool flush = true);
	std::u16string_view Data() const;
	size_t End() const;
	std::u16string_view View(size_t from, size_t to) const;

	void Erase(size_t index);

	static void RightTrim(std::u16string& text);

	static std::u16string U8toU16(std::string_view text);
	static std::string U16toU8(std::u16string_view text);

private:
	UnicodeConverterImpl* _impl;
};

