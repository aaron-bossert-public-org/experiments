# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

file(GLOB_RECURSE DCS_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	)

message("DCS_SRC " ${DCS_SRC})

add_library(dcs STATIC ${DCS_SRC})

target_link_libraries(dcs atmos)

target_include_directories(dcs 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
)