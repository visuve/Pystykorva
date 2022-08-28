#include "PCH.hpp"
#include "EncodingDetector.hpp"

class EncodingDetectorImpl
{
public:
	EncodingDetectorImpl() :
		_detector(ucsdet_open(&_status))
	{
		assert(U_SUCCESS(_status));
	}

	~EncodingDetectorImpl()
	{
		if (_detector)
		{
			ucsdet_close(_detector);
		}
	}

	std::string DetectEncoding(std::string_view sample)
	{
		ucsdet_setText(_detector, sample.data(), static_cast<int32_t>(sample.size()), &_status);
		assert(U_SUCCESS(_status));

		const UCharsetMatch* match = ucsdet_detect(_detector, &_status);
		assert(U_SUCCESS(_status));

		if (!match)
		{
			return "Binary";
		}

		int32_t confidence = ucsdet_getConfidence(match, &_status);
		assert(U_SUCCESS(_status));

		std::string encoding = ucsdet_getName(match, &_status);
		assert(U_SUCCESS(_status));

		if (confidence < 15)
		{
			return "Binary";
		}

		// I do not have any IBM encoded files and I do not care even if I had
		if (encoding.starts_with("IBM"))
		{
			return "Binary";
		}

		// Buggy ICU. Many image files are incorrectly reported as UTF-16BE
		if (confidence <= 30 && encoding == "UTF-16BE")
		{
			return "Binary";
		}

		return encoding;
	}

private:
	UErrorCode _status = U_ZERO_ERROR;
	UCharsetDetector* _detector;
};

EncodingDetector::EncodingDetector() :
	_impl(new EncodingDetectorImpl())
{
}

EncodingDetector::~EncodingDetector()
{
	delete _impl;
}

std::string EncodingDetector::DetectEncoding(std::string_view sample)
{
	return _impl->DetectEncoding(sample);
}
