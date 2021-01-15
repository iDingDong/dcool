add_executable(test_dcool "./test.cpp" "./vigor/sealer.cpp")

target_link_libraries(test_dcool PRIVATE dcool_test)

set_target_properties(test_dcool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")
