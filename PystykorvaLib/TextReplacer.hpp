#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

struct ReplaceException : std::runtime_error
{
	inline ReplaceException(const std::string& message) :
		std::runtime_error(message)
	{
	}
};

class TextReplacerImpl;

class TextReplacer
{
public:
	TextReplacer(const Pystykorva::IFile&, Pystykorva::Result&);
	~TextReplacer();
	NonCopyable(TextReplacer);

	void ReplaceAll(Pystykorva::IFile& output, std::u16string_view);

private:
	TextReplacerImpl* _impl;
};

