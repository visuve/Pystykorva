#pragma once

class MockFile : public Pystykorva::IFile
{
public:
	MockFile(size_t size) :
		_buffer(size, '\0')
	{
	}

	template<typename T, size_t N>
	MockFile(T(&data)[N]) :
		_buffer(sizeof(T)* (N - 1), '\0')
	{
		std::memcpy(_buffer.data(), data, _buffer.size());
	}

	inline ~MockFile()
	{
		_buffer.clear();
	}

	inline uint64_t Size() const override
	{
		return _buffer.size();
	}

	inline std::string_view Sample(size_t size = 0x400) const override
	{
		return { _buffer.data(), std::min(_buffer.size(), size) };
	}

	std::string_view Chunk(uint64_t offset, uint64_t size) const override
	{
		if (offset + size > _buffer.size())
		{
			throw std::out_of_range("chunk out of bounds");
		}
		
		return { _buffer.data() + offset, size };
	}

	inline std::string_view Data() const override
	{
		return _buffer;
	}

	inline void Read(void* data, uint64_t size) override
	{
		if (_offset + size > _buffer.size())
		{
			throw std::out_of_range("read out of bounds");
		}

		for (uint64_t offset = 0; offset < size; ++offset, ++_offset)
		{
			auto source = _buffer.data() + _offset;
			auto target = reinterpret_cast<char*>(data) + offset;
			*target = *source;
		}
	}

	inline void Write(const void* data, uint64_t size) override
	{
		if (_offset + size > _buffer.size())
		{
			throw std::out_of_range("write of bounds");
		}

		for (uint64_t offset = 0; offset < size; ++offset, ++_offset)
		{
			auto source = reinterpret_cast<const char*>(data) + offset;
			auto target = _buffer.data() + _offset;
			*target = *source;
		}
	}

private:
	std::string _buffer;
	mutable size_t _offset = 0;
};
