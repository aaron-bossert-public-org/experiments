# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

file(GLOB THIRD_PARTY_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/gli_wrapper/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/gli_wrapper/src/*.cpp"
	)

add_library(third_party STATIC ${THIRD_PARTY_SRC})

target_include_directories(third_party 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/gli_wrapper/gli"
		"${CMAKE_CURRENT_SOURCE_DIR}/gli_wrapper/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/glm"
	INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/gli_wrapper/include"
)