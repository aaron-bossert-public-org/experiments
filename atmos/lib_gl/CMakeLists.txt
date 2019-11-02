
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)


BUILD_LIB(
	gl
	"batch buffer context defines shader texture utility window")

IF (CMAKE_CL_64)

	target_include_directories(gl
		PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN64/include"
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/angle-master/include

		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR})

	target_link_libraries(gl 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN64/lib-vc2015/glfw3.lib")

ELSE()

	target_include_directories(gl
		PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN32/inlcude"
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/angle-master/include

		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR})

	target_link_libraries(gl 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib")

ENDIF()


target_link_libraries(gl igpu framework)
