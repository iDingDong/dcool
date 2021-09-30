#ifndef DCOOL_CORE_TEST_CHECK_HPP_INCLUDED_
#	define DCOOL_CORE_TEST_CHECK_HPP_INCLUDED_

#	include <dcool/test/case.hpp>

#	include <utility>

#	define DCOOL_TEST_CHECK(level_, predicate_) \
	::dcool::test::detail_::check_(__FILE__, __LINE__, (level_), (predicate_), dcoolTestRecord_)

#	define DCOOL_TEST_EXPECT(predicate_) DCOOL_TEST_CHECK(::dcool::test::Case::Failure::Level::gentle, (predicate_))

#	define DCOOL_TEST_ASSERT(predicate_) DCOOL_TEST_CHECK(::dcool::test::Case::Failure::Level::fatalForCase, (predicate_))

#	define DCOOL_TEST_CHECK_RANGE_EQUALITY(level_, left_, right_) \
	::dcool::test::detail_::checkRangeEquality_(__FILE__, __LINE__, (level_), (left_), (right_), dcoolTestRecord_)

#	define DCOOL_TEST_EXPECT_RANGE_EQUALITY(left_, right_) \
	DCOOL_TEST_CHECK_RANGE_EQUALITY(::dcool::test::Case::Failure::Level::gentle, (left_), (right_))

#	define DCOOL_TEST_ASSERT_RANGE_EQUALITY(left_, right_) \
	DCOOL_TEST_CHECK_RANGE_EQUALITY(::dcool::test::Case::Failure::Level::fatalForCase, (left_), (right_))

#	define DCOOL_TEST_SEQUENCE(...) ::std::initializer_list { __VA_ARGS__ }

#	define DCOOL_TEST_SEQUENCE_OF(Type, ...) ::std::initializer_list<Type>{ __VA_ARGS__ }

#	define DCOOL_TEST_EXPECT_THROW(ExceptionType, ...) \
	do { \
		dcool::core::Boolean dcoolTestThrown_ = false; \
		try __VA_ARGS__ catch (ExceptionType) { \
			dcoolTestThrown_ = true; \
		} \
		DCOOL_TEST_EXPECT(dcoolTestThrown_); \
	} while (false)

#define DCOOL_TEST_CONTEXT_PARAMETER \
	::dcool::test::Case::ActiveRecord& dcoolTestRecord_, \
	::dcool::test::Name dcoolTestSuiteName_, \
	::dcool::test::Name dcoolTestCaseName_
#define DCOOL_TEST_CONTEXT_ARGUMENT ::std::ref(dcoolTestRecord_), dcoolTestSuiteName_, dcoolTestCaseName_

#define DCOOL_TEST_CAPTURE_CONTEXT &dcoolTestRecord_, dcoolTestSuiteName_, dcoolTestCaseName_

#define DCOOL_TEST_SUITE_NAME dcoolTestSuiteName_
#define DCOOL_TEST_CASE_NAME dcoolTestCaseName_

#endif