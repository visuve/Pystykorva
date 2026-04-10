file(GLOB SourceFiles "*.?pp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(LibraryName "LibPystykorva")
else()
	set(LibraryName "libpystykorva")
endif()

add_library(${LibraryName} STATIC ${SourceFiles})
target_precompile_headers(${LibraryName} PRIVATE "PystykorvaLib.pch")

