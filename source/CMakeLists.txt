set(
	dcool_base_sources
	""
)

set(
	dcool_base_exportation_sources
	""
)

if(dcool_might_use_modules AND dcool_base_exportation_sources)
	list(INSERT dcool_base_sources 0 ${dcool_base_exportation_sources})
endif()

if(dcool_base_sources)
	add_library(dcool_base SHARED ${dcool_base_sources})
	set(dcool_base_scope PUBLIC)
else()
	add_library(dcool_base INTERFACE)
	set(dcool_base_scope INTERFACE)
endif()

target_compile_features(dcool_base ${dcool_base_scope} cxx_std_20)
target_enable_common_warnings(dcool_base ${dcool_base_scope})
target_compile_definitions(dcool_base ${dcool_base_scope} DCOOLER_CPP_P1135R6_ENABLED=1)
target_include_directories(dcool_base ${dcool_base_scope} ${dcool_include_directory})

target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_DEBUG} "DCOOLER_ENABLE_DEBUG")
target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_AGGRESSIVE_DEBUG} "DCOOLER_ENABLE_AGGRESSIVE_DEBUG")
target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_CPP_P0024R2} "DCOOLER_ENABLE_CPP_P0024R2")
target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_CPP_P1001R2} "DCOOLER_ENABLE_CPP_P1001R2")
target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_CPP_P1103R3} "DCOOLER_ENABLE_CPP_P1103R3")
target_apply_switch_config(dcool_base ${dcool_base_scope} ${DCOOL_CPP_P1135R6} "DCOOLER_ENABLE_CPP_P1135R6")

if(dcool_might_use_modules)
	target_compile_option_if_supported(dcool_base ${dcool_base_scope} "-fmodules-ts")
endif()
if(dcool_might_need_tbb)
	target_link_library_if_exist(dcool_base ${dcool_base_scope} tbb)
endif()
target_link_library_if_exist(dcool_base ${dcool_base_scope} atomic)

add_subdirectory("./concurrency")
add_subdirectory("./core")
add_subdirectory("./coroutine")
add_subdirectory("./test")
