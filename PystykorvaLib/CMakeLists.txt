if(MSVC)
	set(ICU_INCLUDE_DIR "$ENV{WindowsSdkDir}Include\\$ENV{WindowsSDKLibVersion}um")
endif()

# apt install libicu-dev / dnf install icu
find_package(ICU COMPONENTS i18n uc REQUIRED)

file(GLOB SourceFiles "*.cpp")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(LibraryName "LibPystykorva")
else()
	set(LibraryName "libpystykorva")
endif()

add_library(${LibraryName} STATIC ${SourceFiles})
target_precompile_headers(${LibraryName} PRIVATE "PystykorvaLib.pch")



target_link_libraries(${LibraryName} LINK_PUBLIC ICU::i18n ICU::uc)
