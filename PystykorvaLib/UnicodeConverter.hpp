#pragma once

class UnicodeConverterImpl;

class UnicodeConverter
{
public:
	UnicodeConverter(std::string_view encoding);
	~UnicodeConverter();

	void Convert(std::string_view sample, bool flush);
	std::u16string_view Data() const;
	std::u16string_view View(size_t from, size_t to) const;

	void Erase(size_t index);

private:
	UnicodeConverterImpl* _impl;
};

