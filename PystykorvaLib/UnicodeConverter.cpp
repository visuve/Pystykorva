#include "PystykorvaLib.pch"
#include "UnicodeConverter.hpp"

class UnicodeConverterImpl
{
public:
	UnicodeConverterImpl(std::string_view encoding) :
		_converter(ucnv_open(encoding.data(), &_status))
	{
		assert(U_SUCCESS(_status));
	}

	~UnicodeConverterImpl()
	{
		if (_converter)
		{
			ucnv_close(_converter);
		}
	}

	NonCopyable(UnicodeConverterImpl);

	uint8_t CharSize() const
	{
		int8_t minCharSize = ucnv_getMinCharSize(_converter);

		if (minCharSize < 0 || minCharSize > 4)
		{
			throw ConversionException("ucnv_getMinCharSize went haywire");
		}

		return static_cast<uint8_t>(minCharSize);
	}

	void Convert(std::string_view sample, bool flush)
	{
		std::u16string buffer(sample.size(), '\0');
		char16_t* target = buffer.data();
		char16_t* targetLimit = buffer.data() + buffer.size();

		const char* sourceData = sample.data();
		const char* sourceDataLimit = sample.data() + sample.size();

		ucnv_toUnicode(_converter, &target, targetLimit, &sourceData, sourceDataLimit, nullptr, flush, &_status);
		
		if (U_FAILURE(_status))
		{
			throw ConversionException("ucnv_toUnicode failed");
		}

		size_t end = targetLimit - target;
		_data.append(buffer, 0, buffer.size() - end);
	}

	std::u16string_view Data() const
	{
		return _data;
	}

	size_t End() const
	{
		// The size of the data equals the last possible index, duh
		return _data.size();
	}

	std::u16string_view View(size_t from, size_t to) const
	{
		assert(from < to);
		// Returning a substring directly from _data adds null to the beginning
		return std::u16string_view(_data).substr(from, to - from);
	}

	void Erase(size_t index)
	{
		_data.erase(0, index);
	}

private:
	UErrorCode _status = U_ZERO_ERROR;
	UConverter* _converter = nullptr;
	std::u16string _data;
};

UnicodeConverter::UnicodeConverter(std::string_view encoding) :
	_impl(new UnicodeConverterImpl(encoding))
{
}

UnicodeConverter::~UnicodeConverter()
{
	delete _impl;
}

uint8_t UnicodeConverter::CharSize() const
{
	return _impl->CharSize();
}

void UnicodeConverter::Convert(std::string_view sample, bool flush)
{
	return _impl->Convert(sample, flush);
}

std::u16string_view UnicodeConverter::Data() const
{
	return _impl->Data();
}

size_t UnicodeConverter::End() const
{
	return _impl->End();
}

std::u16string_view UnicodeConverter::View(size_t from, size_t to) const
{
	return _impl->View(from, to);
}

void UnicodeConverter::Erase(size_t index)
{
	return _impl->Erase(index);
}

void UnicodeConverter::RightTrim(std::u16string& text)
{
	const auto notSpace = [](char16_t x)->bool
	{
		return u_isWhitespace(x) == 0;
	};

	text.erase(std::find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
}

std::u16string UnicodeConverter::U8toU16(std::string_view text)
{
	int32_t required = 0;

	{
		UErrorCode error = U_ZERO_ERROR;

		u_strFromUTF8(
			nullptr,
			0,
			&required,
			text.data(),
			static_cast<int32_t>(text.length()),
			&error);

		assert(error == U_BUFFER_OVERFLOW_ERROR);
		assert(required >= 0);
	}
	
	std::u16string result(static_cast<size_t>(required), '\0');

	{
		UErrorCode error = U_ZERO_ERROR;

		u_strFromUTF8(
			result.data(),
			required,
			&required,
			text.data(),
			static_cast<int32_t>(text.length()),
			&error);

		assert(error == U_STRING_NOT_TERMINATED_WARNING);
		assert(required >= 0);
	}

	return result;
}

std::string UnicodeConverter::U16toU8(std::u16string_view text)
{
	int32_t required = 0;

	{
		UErrorCode error = U_ZERO_ERROR;

		u_strToUTF8(
			nullptr,
			0,
			&required,
			text.data(),
			static_cast<int32_t>(text.length()),
			&error);

		assert(error == U_BUFFER_OVERFLOW_ERROR);
		assert(required >= 0);
	}

	std::string result(static_cast<size_t>(required), '\0');

	{
		UErrorCode error = U_ZERO_ERROR;

		u_strToUTF8(
			result.data(),
			required,
			&required,
			text.data(),
			static_cast<int32_t>(text.length()),
			&error);

		assert(error == U_STRING_NOT_TERMINATED_WARNING);
		assert(required >= 0);
	}

	return result;
}
