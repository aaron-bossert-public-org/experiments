
# specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

BUILD_LIB(
	framework 
	"concurrent logging meta_programming perf tweak utility")

target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

BUILD_LIB(
	framework_tests 
	"concurrent main")

target_include_directories(framework_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(framework_tests framework)