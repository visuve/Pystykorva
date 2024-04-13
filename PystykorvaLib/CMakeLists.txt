file(GLOB SourceFiles "*.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(LibraryName "LibPystykorva")
else()
	set(LibraryName "libpystykorva")
endif()

add_library(${LibraryName} STATIC ${SourceFiles})
target_precompile_headers(${LibraryName} PRIVATE "PCH.hpp")
target_link_libraries(${LibraryName} LINK_PUBLIC ICU::i18n ICU::uc)

