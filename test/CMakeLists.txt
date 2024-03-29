set(
	test_dcool_sources
	"./concurrency/atom.cpp"
	"./concurrency/latch.cpp"
	"./concurrency/mutex.cpp"
	"./concurrency/semaphore.cpp"
	"./concurrency/thread_pool.cpp"
	"./core/integer.cpp"
	"./core/layout.cpp"
	"./core/storage.cpp"
	"./coroutine/async_mutex.cpp"
	"./coroutine/generator.cpp"
	"./coroutine/task.cpp"
	"./container/linked.cpp"
	"./container/list.cpp"
	"./container/string.cpp"
	"./resource/shared.cpp"
	"./service/once_invoker.cpp"
	"./service/sealer.cpp"
	"./service/timer.cpp"
	"./utility/any_mutex.cpp"
	"./utility/any.cpp"
	"./utility/cow.cpp"
	"./utility/function.cpp"
	"./utility/overloaded_function.cpp"
	"./test.cpp"
)

add_executable(
	test_dcool
	${test_dcool_sources}
)

target_link_libraries(test_dcool PRIVATE dcool_core)
target_link_libraries(test_dcool PRIVATE dcool_coroutine)
target_link_libraries(test_dcool PRIVATE dcool_test)

set_target_properties(test_dcool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${dcool_output_directory}")

add_test(NAME test_dcool_all COMMAND test_dcool)
