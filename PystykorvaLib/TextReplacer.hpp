#pragma once

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
	TextReplacer();
	~TextReplacer();

	void ReplaceAll(Pystykorva::IFile&, Pystykorva::Match&, std::string_view);
private:
	TextReplacerImpl* _impl;
};
