#include "PCH.hpp"
#include "EncodingDetector.hpp"

class EncodingDetectorImpl
{
public:
	EncodingDetectorImpl() :
		_detector(ucsdet_open(&_status))
	{
		if (U_FAILURE(_status))
		{
			throw EncodingException("ucsdet_open failed");
		}
	}

	~EncodingDetectorImpl()
	{
		if (_detector)
		{
			ucsdet_close(_detector);
		}
	}

	bool DetectEncoding(std::string_view sample, Pystykorva::EncodingGuess& encoding)
	{
		ucsdet_setText(
			_detector,
			sample.data(),
			std::min<int32_t>(static_cast<int32_t>(sample.size()), 0x800),
			&_status);

		if (U_FAILURE(_status))
		{
			throw EncodingException("ucsdet_setText failed");
		}

		const UCharsetMatch* match = ucsdet_detect(_detector, &_status);

		if (U_FAILURE(_status) || !match)
		{
			throw EncodingException("ucsdet_detect failed");
		}

		encoding.Confidence = ucsdet_getConfidence(match, &_status);

		if (U_FAILURE(_status))
		{
			throw EncodingException("ucsdet_getConfidence failed");
		}

		encoding.Name = ucsdet_getName(match, &_status);
		
		if (U_FAILURE(_status))
		{
			throw EncodingException("ucsdet_getConfidence failed");
		}

		if (encoding.Confidence < 15)
		{
			return false;
		}

		// Sometimes large .img files are detected as UTF-32BE and some .o files as UTF-32LE
		if (encoding.Name.starts_with("UTF-32"))
		{
			return false;
		}

		// Sometimes jpg files are detected as some weird IBM things
		if (encoding.Name.starts_with("IBM"))
		{
			return false;
		}

		// Sometimes executables are detected as KOI8-R
		if (encoding.Confidence <= 50 && encoding.Name.starts_with("KOI"))
		{
			return false;
		}

		// Buggy ICU. Many image files are incorrectly reported as UTF-16BE or windows-1252
		if (encoding.Confidence <= 30 && encoding.Name == "UTF-16BE" || encoding.Name.starts_with("windows"))
		{
			return false;
		}

		return true;
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

bool EncodingDetector::DetectEncoding(std::string_view sample, Pystykorva::EncodingGuess& encoding)
{
	return _impl->DetectEncoding(sample, encoding);
}
