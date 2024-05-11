#include "PystykorvaLib.pch"
#include "TextReplacer.hpp"

class TextReplacerImpl
{
public:
	TextReplacerImpl(const Pystykorva::IFile& input, Pystykorva::Result& result) :
		_input(input),
		_result(result),
		_converter(ucnv_open(result.Encoding.Name.data(), &_status))
	{
		if (U_FAILURE(_status))
		{
			throw ReplaceException("ucnv_open failed");
		}
	}

	~TextReplacerImpl()
	{
		if (_converter)
		{
			ucnv_close(_converter);
		}
	}
	
	NonCopyable(TextReplacerImpl);

	void ReplaceAll(
		Pystykorva::IFile& output,
		std::u16string_view replacement)
	{
		uint64_t offset = 0;

		std::string sourceEncodedReplacement = SourceEncode(replacement);

		for (Pystykorva::Match& match : _result.Matches)
		{
			for (const auto& [relative, absolute] : match.Positions)
			{
				// TODO: progress reporting?

				const uint64_t chunkSize = std::max(absolute.Begin, offset) - std::min(absolute.Begin, offset);

				if (chunkSize)
				{
					auto chunk = _input.Chunk(offset, chunkSize);
					output.Write(chunk.data(), chunkSize);
					offset += chunkSize;
				}

				output.Write(sourceEncodedReplacement.data(), sourceEncodedReplacement.size());
				offset += absolute.Size();

				// In case someone wants to render the end result
				match.LineContent.replace(relative.Begin, relative.Size(), replacement);
			}
		}

		const uint64_t bytesLeft = _input.Size() - offset;

		if (bytesLeft)
		{
			auto chunk = _input.Chunk(offset, bytesLeft);
			output.Write(chunk.data(), bytesLeft);
		}
	}

private:
	std::string SourceEncode(std::u16string_view replacement)
	{
		std::string buffer(replacement.size() * 2, '\0');
		char* target = buffer.data();
		char* targetLimit = buffer.data() + buffer.size();

		const char16_t* source = replacement.data();
		const char16_t* sourceLimit = replacement.data() + replacement.size();

		ucnv_fromUnicode(_converter, &target, targetLimit, &source, sourceLimit, nullptr, true, &_status);

		if (U_FAILURE(_status))
		{
			throw ReplaceException("ucnv_toUnicode failed");
		}

		buffer.resize(targetLimit - target);

		return buffer;
	}

	const Pystykorva::IFile& _input;
	Pystykorva::Result& _result;
	UErrorCode _status = U_ZERO_ERROR;
	UConverter* _converter = nullptr;
};

TextReplacer::TextReplacer(const Pystykorva::IFile& file, Pystykorva::Result& result) :
	_impl(new TextReplacerImpl(file, result))
{
}

TextReplacer::~TextReplacer()
{
	delete _impl;
}

void TextReplacer::ReplaceAll(
	Pystykorva::IFile& output,
	std::u16string_view replacement)
{
	_impl->ReplaceAll(output, replacement);
}
