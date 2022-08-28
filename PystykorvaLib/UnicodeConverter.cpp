#include "PCH.hpp"
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

	void Convert(std::string_view sample, bool flush)
	{
		std::u16string buffer(sample.size(), '\0');
		char16_t* target = buffer.data();
		char16_t* targetLimit = buffer.data() + buffer.size();

		const char* sourceData = sample.data();
		const char* sourceDataLimit = sample.data() + sample.size();

		ucnv_toUnicode(_converter, &target, targetLimit, &sourceData, sourceDataLimit, nullptr, flush, &_status);
		assert(U_SUCCESS(_status));

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
