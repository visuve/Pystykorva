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
	add_compile_definitions(NOMINMAX VC_EXTRALEAN)
endif()

# apt install libicu-dev / dnf install icu
find_package(ICU COMPONENTS i18n uc REQUIRED)

message(${ICU_INCLUDE_DIRS})

add_subdirectory("PystykorvaLib")
include_directories("PystykorvaLib")

if(MSVC)
	# This just uses too new features for GCC or Clang
	add_subdirectory("PystykorvaCLI")
endif()

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
include("GoogleTest.cmake")
include(GoogleTest)
add_subdirectory("PystykorvaTests")
enable_testing()
