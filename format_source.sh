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

DRY_RUN=false
DO_CODE=false
DO_SHADERS=false




while getopts ":dcs" opt; do
  case ${opt} in
   	d )
		DRY_RUN=true
      ;;
    c )
		DO_CODE=true
      ;;
    s )
		DO_SHADERS=true
      ;;
    \? )
      echo "Usage:"
      echo "    -h                 Display this help message."
      echo "    -d                 dry run."
      echo "    -c                 run on source files."
      echo "    -s                 run on shaders."
      echo "Invalid Option: -$OPTARG" 1>&2
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))



task(){
	if [ "$DRY_RUN" = true ] ; then
		${DIR}/clang-format.exe -style=file --dry-run $file
	else
		${DIR}/clang-format.exe -style=file -i $file &
	fi
}

N=16
(
	if [ "$DO_CODE" = true ] ; then
		for FORMAT_TARGET in "${FRAMEWORK_DIR}" "${FRAMEWORK_TESTS_DIR}" "${GFX_DIR}" "${GL_DIR}" "${IGPU_DIR}" "${VULKAN_DIR}" "${DCS_TEST_DIR}"
		do
		
			echo "Format code in directory: ${FORMAT_TARGET} ..."
			cd $"${FORMAT_TARGET}"
			for file in **/*.{cpp,h} 
			do 
			   ((i=i%N)); 
			   ((i++==0)) && wait
		   		task "$file" 
			done
		done
	fi

	if [ "$DO_SHADERS" = true ] ; then
		for FORMAT_TARGET in "${DCS_RAW_ASSETS}"
		do
		
			echo "Format shaders in directory: ${FORMAT_TARGET} ..."
			cd $"${FORMAT_TARGET}"
			for file in **/*.{vert,frag,comp} 
			do 
			   ((i=i%N)); 
			   ((i++==0)) && wait
		   		task "$file"
			done
		done
	fi
)
