#pragma once

template<typename T>
concept Numeric =
	std::is_same_v<int16_t, T> ||
	std::is_same_v<int32_t, T> ||
	std::is_same_v<int32_t, T> ||
	std::is_same_v<uint8_t, T> ||
	std::is_same_v<uint16_t, T> ||
	std::is_same_v<uint32_t, T> ||
	std::is_same_v<uint32_t, T> ||
	std::is_same_v<size_t, T>;

template<typename T>
concept NarrowCharacter = std::is_same_v<char, T> || std::is_same_v<char8_t, T>;

template<typename T>
concept WideCharacter =  std::is_same_v<wchar_t, T> || std::is_same_v<char16_t, T>;


template<typename T>
concept Path = requires(T param)
{
	requires std::is_same_v<std::filesystem::path, T>;
};

// I could not write a concept for this
struct RawString
{
	char const* Pointer;
	size_t Size;

	RawString(const char* value) :
		Pointer(value),
		Size(value ? strlen(value) : 0)
	{
	}
};

class Console
{
public:
	enum OutputType : int
	{
		StandardOutput = 1,
		StandardError = 2
	};

	explicit Console(OutputType);
	~Console();

	Console& operator << (Numeric auto value)
	{
		const std::string str = std::to_string(value);
		WriteA(str.data(), str.size());
		return *this;
	}

	Console& operator << (NarrowCharacter auto value)
	{
		WriteA(&value, sizeof(value));
		return *this;
	}

	template <typename T>
	Console& operator << (const std::basic_string<T>& value) requires NarrowCharacter<T>
	{
		WriteA(value.data(), value.size());
		return *this;
	}

	template <typename T>
	Console& operator << (const std::basic_string<T>& value) requires WideCharacter<T>
	{
		WriteW(value.data(), value.size());
		return *this;
	}

	template <typename T>
	Console& operator << (std::basic_string_view<T> value) requires NarrowCharacter<T>
	{
		WriteA(value.data(), value.size());
		return *this;
	}

	template <typename T>
	Console& operator << (std::basic_string_view<T> value) requires WideCharacter<T>
	{
		WriteW(value.data(), value.size());
		return *this;
	}

	template <size_t N>
	Console& operator << (const char(&value)[N])
	{
		WriteA(value, N);
		return *this;
	}

	inline Console& operator << (const RawString& value)
	{
		WriteA(value.Pointer, value.Size);
		return *this;
	}

	Console& operator << (const Path auto& value)
	{
#ifdef _WIN32
		const std::wstring str = value;
		WriteW(str.data(), str.size());
#else
		const std::string str = value;
		WriteW(str.data(), str.size());
#endif
		return *this;
	}

	template <class Rep, class Period>
	inline Console& operator << (std::chrono::duration<Rep, Period> time)
	{
		const std::string str = std::format("{}", time);
		WriteA(str.data(), str.size());
		return *this;
	}

private:
	void WriteA(const void* data, size_t size) const;
	void WriteW(const void* data, size_t size) const;

	void* _stream;
};