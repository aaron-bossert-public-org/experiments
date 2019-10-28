
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

BUILD_LIB_WARNINGS_ALLOWED(
	igpu_third_party_builtins
	"pvr/tools SPIRV-Cross")

target_include_directories(
	igpu_third_party_builtins
	
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/pvr/tools
		${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross

	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

