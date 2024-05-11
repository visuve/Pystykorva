#pragma once

#include "NonCopyable.hpp"
#include "Pystykorva.hpp"

class MemoryMappedFileImpl;

struct IOException : std::system_error
{
	IOException(const std::string& message);
};

class MemoryMappedFile : public Pystykorva::IFile
{
public:
	MemoryMappedFile(const std::filesystem::path& path, uint64_t size, bool readOnly);
	~MemoryMappedFile();
	NonCopyable(MemoryMappedFile);

	virtual uint64_t Size() const override;
	std::string_view Sample(uint64_t size) const override;
	std::string_view Chunk(uint64_t from, uint64_t size) const override;
	std::string_view Data() const override;
	void Read(void* data, uint64_t size) override;
	void Write(const void* data, uint64_t size) override;

private:
	MemoryMappedFileImpl* _impl;
	uint64_t _offset = 0;
};

