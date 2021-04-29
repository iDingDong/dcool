set(
	dcool_coroutine_sources
	"./async_mutex.cpp"
)

add_library(
	dcool_coroutine
	SHARED
	${dcool_coroutine_sources}
)

target_compile_features(dcool_coroutine PUBLIC cxx_std_20)
target_enable_common_warnings(dcool_coroutine PUBLIC)
target_enable_pedantic(dcool_coroutine PRIVATE)
target_compile_option_if_supported(dcool_coroutine PUBLIC "-fcoroutines")

target_link_library_if_exist(dcool_coroutine PUBLIC tbb)

target_include_directories(dcool_coroutine PUBLIC "${DCOOL_INCLUDE_DIRECTORY}")

set_target_properties(dcool_coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

add_library(
	dcooldebug_coroutine
	SHARED
	${dcool_coroutine_sources}
)

target_compile_features(dcooldebug_coroutine PUBLIC cxx_std_20)
target_enable_common_warnings(dcooldebug_coroutine PUBLIC)
target_enable_pedantic(dcooldebug_coroutine PRIVATE)
target_compile_option_if_supported(dcooldebug_coroutine PUBLIC "-fcoroutines")

target_link_library_if_exist(dcooldebug_coroutine PUBLIC tbb)

target_include_directories(dcooldebug_coroutine PUBLIC "${DCOOL_INCLUDE_DIRECTORY}")

set_target_properties(dcooldebug_coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

target_compile_definitions(dcooldebug_coroutine PUBLIC DCOOLER_CORE_ENABLE_AGGRESSIVE_DEBUG=1)
