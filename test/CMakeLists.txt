add_executable(
	test_dcool
	"./test.cpp"
	"./container/linked.cpp"
	"./container/list.cpp"
	"./container/string.cpp"
	"./resource/shared.cpp"
	"./utility/any.cpp"
	"./utility/function.cpp"
	"./utility/overloaded_function.cpp"
	"./vigor/sealer.cpp"
)

target_link_libraries(test_dcool PRIVATE dcool_test)

set_target_properties(test_dcool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")

add_executable(
	test_dcooldebug
	"./test.cpp"
	"./container/linked.cpp"
	"./container/list.cpp"
	"./container/string.cpp"
	"./resource/shared.cpp"
	"./utility/any.cpp"
	"./utility/function.cpp"
	"./utility/overloaded_function.cpp"
	"./vigor/sealer.cpp"
)

target_link_libraries(test_dcooldebug PRIVATE dcooldebug_test)

set_target_properties(test_dcooldebug PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DCOOL_OUTPUT_DIRECTORY}")
