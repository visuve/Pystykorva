#include "PystykorvaLib.pch"
#include "TextReplacer.hpp"

class TextReplacerImpl
{
public:
	TextReplacerImpl()
	{
		// TODO!
	}

	~TextReplacerImpl()
	{
	}
	
	NonCopyable(TextReplacerImpl);

private:
};

TextReplacer::TextReplacer() :
	_impl(new TextReplacerImpl())
{
}

TextReplacer::~TextReplacer()
{
}


void TextReplacer::ReplaceAll(Pystykorva::IFile&, Pystykorva::Match&, std::string_view)
{
	// TODO!
}
