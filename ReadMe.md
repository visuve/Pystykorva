# Pystykorva

- A C++ library containing Grep-like functionality
	- Implemented using Boost, with ICU as its backend

## Development

- Point ``ICU_PATH`` environment variable to ICU root e.g. ``X:\\icu4c-71_1``
	- https://icu.unicode.org/download
- Compile Boost (static linkage) with test and localization modules with ICU
	- https://www.boost.org/users/download/
- Point ``BOOST_INCLUDEDIR`` environment variable to Boost root e.g. ``X:\boost_1_80_0``
- Point ``BOOST_LIBRARYDIR`` environment variable to Boost stage folder e.g. ``X:\boost_1_80_0\stage\lib``

## Design principles

- When a similar or same functionality is found in STL, use it rather than Boost

## TODO: 

- Consider using vcpkg with the project