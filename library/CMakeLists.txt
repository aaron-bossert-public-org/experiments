# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(library STATIC empty.cpp)

add_subdirectory(atmos)
add_subdirectory(dcs)

target_link_libraries(library atmos)
target_link_libraries(library dcs)

target_include_directories(library 
	INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}"
)

WARNINGS_AS_ERRORS(library)