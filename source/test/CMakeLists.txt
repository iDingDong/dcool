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
