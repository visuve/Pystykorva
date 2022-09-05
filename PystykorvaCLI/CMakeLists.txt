file(GLOB SourceFiles "*.cpp")

add_executable("pystykorva" ${SourceFiles})
target_precompile_headers("pystykorva" PRIVATE "PCH.hpp")
target_link_libraries("pystykorva" LINK_PRIVATE libpystykorva ICU::uc)
