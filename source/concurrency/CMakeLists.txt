set(
	cdcool_concurrency_sources
	"./c_mutex.cpp"
	"./c_thread.cpp"
)

add_library(
	cdcool_concurrency
	SHARED
	${cdcool_concurrency_sources}
)

target_enable_pedantic(cdcool_concurrency PRIVATE)

target_link_libraries(cdcool_concurrency PUBLIC dcool_header)

set_target_properties(cdcool_concurrency PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")
