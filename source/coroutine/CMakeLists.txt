set(
	dcool_coroutine_sources
	"./async_mutex.cpp"
)

add_library(
	dcool_coroutine
	SHARED
	${dcool_coroutine_sources}
)

target_enable_pedantic(dcool_coroutine PRIVATE)
target_compile_option_if_supported(dcool_coroutine PUBLIC "-fcoroutines")

target_link_libraries(dcool_coroutine PUBLIC dcool_header)

set_target_properties(dcool_coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

add_library(
	dcooldebug_coroutine
	SHARED
	${dcool_coroutine_sources}
)

target_enable_pedantic(dcooldebug_coroutine PRIVATE)
target_compile_option_if_supported(dcooldebug_coroutine PUBLIC "-fcoroutines")

target_link_libraries(dcooldebug_coroutine PUBLIC dcool_header)

set_target_properties(dcooldebug_coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

target_compile_definitions(dcooldebug_coroutine PUBLIC DCOOLER_CORE_ENABLE_AGGRESSIVE_DEBUG=1)
