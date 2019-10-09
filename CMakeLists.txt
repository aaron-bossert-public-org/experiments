
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)
# CMAKE_POLICY(SET CMP0072 NEW)
project(experiments)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS OFF)

STRING(REPLACE "/O2" "/Od" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO}) 
STRING(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO}) 
STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO}) 
STRING(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO}) 

function(set_sources target source_folders)
    separate_arguments(source_folders WINDOWS_COMMAND "${source_folders}")
    foreach(folder ${source_folders})
        file(GLOB files
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.cpp"
            )
        set(results ${results} ${files})
    endforeach()

    set(sources ${results} PARENT_SCOPE)
endfunction()

function(configure_target target source_folders)

    separate_arguments(source_folders WINDOWS_COMMAND "${source_folders}")
	
	add_definitions(-DGLM_FORCE_SILENT_WARNINGS=1)
    add_definitions(-DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES=1)

    if(MSVC)

        add_definitions(-DATMOS_BUILD_WINDOWS=1)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
        
        target_compile_options(${target} PRIVATE /W4 /WX)

    else()
        
        add_definitions(-DATMOS_BUILD_OSX=1)

        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)

    endif()

    target_compile_options(${target} PRIVATE "$<$<CONFIG:DEBUG>:/DATMOS_DEBUG=1>")
    target_compile_options(${target} PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:/DATMOS_DEBUG=1>")

    # add source folders to project, in MSVC this means addings filters corrisponding to each source folder.
    foreach(folder ${source_folders})
        file(GLOB files
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/${target}/${folder}/*.cpp"
            )

        STRING(REPLACE "/" "\\" group ${folder})
        source_group(${group} FILES ${files})
endforeach()
endfunction()

function(BUILD_LIB lib source_folders)

    set_sources(${lib} ${source_folders})

    add_library(${lib} ${sources})

    configure_target(${lib} ${source_folders})
endfunction()

function(BUILD_EXE exe source_folders)
    
    set_sources(${exe} ${source_folders})

    add_executable(${exe} ${sources})

    configure_target(${exe} ${source_folders})
endfunction()


# dcs_test
set(source_folders "include src")
BUILD_EXE(dcs_test ${source_folders})

target_include_directories(dcs_test 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# atmos
add_subdirectory(atmos)
target_link_libraries(dcs_test vulkan framework_tests)

set_property(TARGET dcs_test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT dcs_test)
