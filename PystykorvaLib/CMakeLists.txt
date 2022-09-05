file(GLOB SourceFiles "*.cpp")

add_library("libpystykorva" STATIC ${SourceFiles})

target_precompile_headers("libpystykorva" PRIVATE "PCH.hpp")

target_link_libraries("libpystykorva" LINK_PUBLIC ICU::i18n ICU::uc)
