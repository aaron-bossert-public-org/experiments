#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

FRAMEWORK_DIR="${DIR}/atmos/lib_framework/framework"
FRAMEWORK_TESTS_DIR="${DIR}/atmos/lib_framework/framework_tests"
GFX_DIR="${DIR}/atmos/lib_gfx/gfx"
GL_DIR="${DIR}/atmos/lib_gl/gl"
IGPU_DIR="${DIR}/atmos/lib_igpu/igpu"
VULKAN_DIR="${DIR}/atmos/lib_vulkan/vulkan"
DCS_TEST_DIR="${DIR}/dcs_test"
DCS_RAW_ASSETS="${DIR}/dcs_test/raw_assets"

while getopts ":hsa" opt; do
  case ${opt} in
    h )
      echo "Usage:"
      echo "    -h                 Display this help message."
      echo "    -s                 run on source files."
      echo "    -a                 run on asset files."
      exit 0
      ;;
    s )
		FOLDERS=( ${FOLDERS[@]} "${FRAMEWORK_DIR}" "${FRAMEWORK_TESTS_DIR}" "${GFX_DIR}" "${GL_DIR}" "${IGPU_DIR}" "${VULKAN_DIR}" "${DCS_TEST_DIR}" )
      ;;
    a )
		FOLDERS=( ${FOLDERS[@]} "${DCS_RAW_ASSETS}" )
      ;;
    \? )
      echo "Invalid Option: -$OPTARG" 1>&2
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))



task(){
	echo "Formatting file: $file ..."
	${DIR}/clang-format.exe -style=file -i $file
}

N=8
(
	for FORMAT_TARGET in ${FOLDERS[@]}
	do
# "${FRAMEWORK_DIR}" "${FRAMEWORK_TESTS_DIR}" "${GFX_DIR}" "${GL_DIR}" "${IGPU_DIR}" "${VULKAN_DIR}" "${DCS_TEST_DIR}" "${DCS_RAW_ASSETS}"
	
		echo "switch directory ${FORMAT_TARGET} ..."
		cd $"${FORMAT_TARGET}"
		for file in **/*.{cpp,h,vert,frag} 
		do 
		   ((i=i%N)); 
		   ((i++==0)) && wait
	   		task "$file" & 
		done

		wait
	done
)
