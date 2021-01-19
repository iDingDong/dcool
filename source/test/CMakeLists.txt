add_library(
	dcool_test
	SHARED
	"./case.cpp"
	"./center.cpp"
	"./suite.cpp"
)

target_compile_features(dcool_test PUBLIC cxx_std_20)

target_include_directories(dcool_test PUBLIC "${DCOOL_INCLUDE_DIRECTORY}")
