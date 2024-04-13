#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

class MemoryMappedFileImpl;

// This interface has no other meaning than to allow testing

class MemoryMappedFile : public Pystykorva::IFile
{
public:
	MemoryMappedFile(const std::filesystem::path&, uint64_t);
	~MemoryMappedFile();
	NonCopyable(MemoryMappedFile);

	std::string_view Sample(size_t size = 0x400) const override;
	std::string_view Data() const override;

private:
	MemoryMappedFileImpl* _impl;
};

