include(FetchContent)

FetchContent_Declare(googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
	URL_HASH SHA256=1f357c27ca988c3f7c6b4bf68a9395005ac6761f034046e9dde0896e3aba00e4
)

FetchContent_MakeAvailable(googletest)
