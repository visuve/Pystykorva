cmake_minimum_required(VERSION 3.16)

project("Pystykorva" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "..")

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	add_compile_options(/MP /W4 /WX)
endif()

add_subdirectory("PystykorvaLib")
include_directories("PystykorvaLib")
add_subdirectory("PystykorvaCLI")

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
include("GoogleTest.cmake")
include(GoogleTest)
add_subdirectory("PystykorvaTests")
enable_testing()
