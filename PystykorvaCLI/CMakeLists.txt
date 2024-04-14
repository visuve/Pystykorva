file(GLOB SourceFiles "*.?pp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(ExecutableName "Pystykorva")
	set(LibraryName "LibPystykorva")
else()
	set(ExecutableName "pystykorva")
	set(LibraryName "libpystykorva")
endif()

add_executable(${ExecutableName} ${SourceFiles})
target_precompile_headers(${ExecutableName} PRIVATE "PystykorvaCLI.pch")
target_link_libraries(${ExecutableName} LINK_PRIVATE ${LibraryName})

