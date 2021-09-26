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

target_link_libraries(dcool_coroutine PUBLIC dcool_base)

set_target_properties(dcool_coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${dcool_output_directory}")
