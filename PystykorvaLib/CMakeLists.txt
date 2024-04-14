if(NOT MSVC)
	# apt install libicu-dev / dnf install icu
	find_package(ICU COMPONENTS i18n uc REQUIRED)
	message(${ICU_INCLUDE_DIR})
endif()

file(GLOB SourceFiles "*.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(LibraryName "LibPystykorva")
else()
	set(LibraryName "libpystykorva")
endif()

add_library(${LibraryName} STATIC ${SourceFiles})
target_precompile_headers(${LibraryName} PRIVATE "PystykorvaLib.pch")

if(MSVC)
	target_link_libraries(${LibraryName} LINK_PUBLIC icu)
else()
	target_link_libraries(${LibraryName} LINK_PUBLIC ICU::i18n ICU::uc)
endif()

