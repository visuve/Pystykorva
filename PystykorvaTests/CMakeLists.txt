file(GLOB SourceFiles "*.cpp")

add_executable("pystykorva_tests" ${SourceFiles})

add_test(NAME "pystykorva_tests" COMMAND $<TARGET_FILE:PystykorvaTests>)

# target_precompile_headers("pystykorva_tests" PRIVATE "PCH.hpp")
target_link_libraries("pystykorva_tests" LINK_PRIVATE libpystykorva gtest)
