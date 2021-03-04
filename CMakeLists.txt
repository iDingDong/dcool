cmake_minimum_required(VERSION 3.18)

project(dcool VERSION 0.6.1 LANGUAGES "CXX")

set(DCOOL_ROOT_DIRECTORY "${PROJECT_SOURCE_DIR}")
set(DCOOL_INCLUDE_DIRECTORY "${DCOOL_ROOT_DIRECTORY}/include")
set(DCOOL_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/binary")

include("CheckCXXCompilerFlag")

function(target_compile_option_if_supported TARGET SCOPE FLAG)
	check_cxx_compiler_flag("${FLAG}" SUPPORTED)
	if(SUPPORTED)
		target_compile_options("${TARGET}" "${SCOPE}" "${FLAG}")
	endif()
	unset(SUPPORTED CACHE)
endfunction()

function(target_enable_common_warnings TARGET SCOPE)
	target_compile_option_if_supported("${TARGET}" "${SCOPE}" "-Wall")
	target_compile_option_if_supported("${TARGET}" "${SCOPE}" "/W4")
endfunction()

function(target_enable_pedantic TARGET SCOPE)
	target_compile_option_if_supported("${TARGET}" "${SCOPE}" "-pedantic")
endfunction()

function(target_link_library_if_exist TARGET SCOPE LIBRARY_NAME)
	find_library(LIBRARY_PATH "${LIBRARY_NAME}")
	if(LIBRARY_PATH)
		target_link_libraries("${TARGET}" "${SCOPE}" "${LIBRARY_PATH}")
	endif()
	unset(LIBRARY_PATH CACHE)
endfunction()

add_subdirectory("./source")
add_subdirectory("./test")
