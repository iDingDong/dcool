cmake_minimum_required(VERSION 3.18)

project(dcool VERSION 0.12.1 LANGUAGES "CXX")

enable_testing()

set(dcool_root_directory "${PROJECT_SOURCE_DIR}")
set(dcool_include_directory "${dcool_root_directory}/include")
set(dcool_output_directory "${CMAKE_BINARY_DIR}/binary")

include("CheckCXXCompilerFlag")

function(target_compile_option_if_supported target scope flag)
	check_cxx_compiler_flag("${flag}" supported)
	if(supported)
		target_compile_options("${target}" "${scope}" "${flag}")
	endif()
	unset(supported CACHE)
endfunction()

function(target_enable_common_warnings target scope)
	target_compile_option_if_supported("${target}" "${scope}" "-Wall")
	target_compile_option_if_supported("${target}" "${scope}" "/W4")
endfunction()

function(target_enable_pedantic target scope)
	target_compile_option_if_supported("${target}" "${scope}" "-pedantic")
endfunction()

function(target_link_library_if_exist target scope library_name)
	find_library(library_path "${library_name}")
	if(library_path)
		target_link_libraries("${target}" "${scope}" "${library_path}")
	endif()
	unset(library_path CACHE)
endfunction()

function(target_apply_switch_config target scope config_value macro_name)
	if(config_value STREQUAL "on")
		target_compile_definitions("${target}" "${scope}" "${macro_name}=1")
	elseif(config_value STREQUAL "off")
		target_compile_definitions("${target}" "${scope}" "${macro_name}=0")
	endif()
endfunction()

set(DCOOL_DEBUG "default" CACHE STRING "Build dcool with debug information with performance penalty")
set_property(CACHE DCOOL_DEBUG PROPERTY STRINGS "default" "on" "off")
message("DCOOL_DEBUG ${DCOOL_DEBUG}")

set(DCOOL_AGGRESSIVE_DEBUG "default" CACHE STRING "Build dcool with more debug information with even more performance penalty")
set_property(CACHE DCOOL_AGGRESSIVE_DEBUG PROPERTY STRINGS "default" "on" "off")
message("DCOOL_AGGRESSIVE_DEBUG ${DCOOL_AGGRESSIVE_DEBUG}")

set(DCOOL_CPP_P0024R2 "default" CACHE STRING "Build dcool with ISO C++ P0024R2 support")
set_property(CACHE DCOOL_CPP_P0024R2 PROPERTY STRINGS "default" "on" "off")
message("DCOOL_CPP_P0024R2 ${DCOOL_CPP_P0024R2}")

set(DCOOL_CPP_P1001R2 "default" CACHE STRING "Build dcool with ISO C++ P1001R2 support")
set_property(CACHE DCOOL_CPP_P1001R2 PROPERTY STRINGS "default" "on" "off")
message("DCOOL_CPP_P1001R2 ${DCOOL_CPP_P1001R2}")

# Change the default value to `default` once cmake and at least one compiler supports all module features we need.
set(DCOOL_CPP_P1103R3 "off" CACHE STRING "Build dcool with ISO C++ P1103R3 support")
set_property(CACHE DCOOL_CPP_P1103R3 PROPERTY STRINGS "default" "on" "off")
message("DCOOL_CPP_P1103R3 ${DCOOL_CPP_P1103R3}")

set(DCOOL_CPP_P1135R6 "default" CACHE STRING "Build dcool with ISO C++ P1135R6 support")
set_property(CACHE DCOOL_CPP_P1135R6 PROPERTY STRINGS "default" "on" "off")
message("DCOOL_CPP_P1135R6 ${DCOOL_CPP_P1135R6}")

set(dcool_might_need_tbb TRUE)
if(DCOOL_CPP_P0024R2 STREQUAL "off" AND DCOOL_CPP_P1001R2 STREQUAL "off")
	set(dcool_might_need_tbb FALSE)
endif()

set(dcool_might_use_modules TRUE)
if(DCOOL_CPP_P1103R3 STREQUAL "off")
	set(dcool_might_use_modules FALSE)
endif()

add_subdirectory("./source")
add_subdirectory("./test")
