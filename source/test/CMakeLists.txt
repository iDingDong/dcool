set(
	dcool_test_sources
	"./case.cpp"
	"./center.cpp"
	"./suite.cpp"
)

add_library(
	dcool_test
	SHARED
	${dcool_test_sources}
)

target_enable_pedantic(dcool_test PRIVATE)

target_link_libraries(dcool_test PUBLIC dcool_header)

set_target_properties(dcool_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

add_library(
	dcooldebug_test
	SHARED
	${dcool_test_sources}
)

target_enable_pedantic(dcooldebug_test PRIVATE)

target_link_libraries(dcooldebug_test PUBLIC dcool_header)
target_link_library_if_exist(dcooldebug_test PUBLIC tbb)

set_target_properties(dcooldebug_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

target_compile_definitions(dcooldebug_test PUBLIC DCOOLER_CORE_ENABLE_AGGRESSIVE_DEBUG=1)
