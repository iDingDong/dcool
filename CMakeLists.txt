cmake_minimum_required(VERSION 3.18)

project(dcool VERSION 0.9.0 LANGUAGES "CXX")

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

add_library(dcool_header INTERFACE)

target_compile_features(dcool_header INTERFACE cxx_std_20)
target_enable_common_warnings(dcool_header INTERFACE)
target_compile_definitions(dcool_header INTERFACE DCOOLER_CPP_P1135R6_ENABLED=1)
target_include_directories(dcool_header INTERFACE ${DCOOL_INCLUDE_DIRECTORY})
target_link_library_if_exist(dcool_header INTERFACE tbb)

add_subdirectory("./source")
add_subdirectory("./test")
