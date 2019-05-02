# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)
# CMAKE_POLICY(SET CMP0072 NEW)
# Project name

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB DCS_TEST_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        )

add_executable(dcs_test ${DCS_TEST_SRCS})

target_link_libraries(dcs_test library)

set_property(TARGET dcs_test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

WARNINGS_AS_ERRORS(dcs_test)