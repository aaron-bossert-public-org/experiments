
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)


BUILD_LIB(
	igpu
	"batch buffer compute context shader texture utility window")

target_include_directories(igpu
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/stb-master

	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glm
)

target_link_libraries(
	igpu 
	
	PUBLIC
	framework 
	
	PRIVATE 
	igpu_third_party_builtins)