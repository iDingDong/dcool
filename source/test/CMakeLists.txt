add_library(
	dcool_test
	SHARED
	"./case.cpp"
	"./center.cpp"
	"./suite.cpp"
)

target_compile_features(dcool_test PUBLIC cxx_std_20)
target_enable_common_warnings(dcool_test PUBLIC)
target_enable_pedantic(dcool_test PRIVATE)

target_compile_options(dcool_test PRIVATE "-fconcepts-diagnostics-depth=3")

target_link_library_if_exist(dcool_test PUBLIC tbb)

target_include_directories(dcool_test PUBLIC "${DCOOL_INCLUDE_DIRECTORY}")

set_target_properties(dcool_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")
