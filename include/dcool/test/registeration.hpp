#ifndef DCOOL_TEST_REGISTERATION_HPP_INCLUDED_
#	define DCOOL_TEST_REGISTERATION_HPP_INCLUDED_ 1

#	include <dcool/test/case.hpp>
#	include <dcool/test/center.hpp>

#define DCOOL_TEST_TOKEN_CAT_3_HELPER_(leading_, middle_, following_) leading_##middle_##following_
#define DCOOL_TEST_TOKEN_CAT_3_(leading_, middle_, following_) DCOOL_TEST_TOKEN_CAT_3_HELPER_(leading_, middle_, following_)

#define DCOOL_TEST_CASE(suiteName_, caseName_) \
		static void DCOOL_TEST_TOKEN_CAT_3_( \
			dcoolImplementationTestExecutor, suiteName_, caseName_ \
		)(::dcool::test::Case::Record& dcoolTestRecord); \
		static int DCOOL_TEST_TOKEN_CAT_3_( \
			dcoolImplementationTestExecutorTestCaseHelper, suiteName_, caseName_ \
		) = ::dcool::test::detail_::registerCase_( \
			#suiteName_, #caseName_, DCOOL_TEST_TOKEN_CAT_3_(dcoolImplementationTestExecutor, suiteName_, caseName_) \
		); \
		static void DCOOL_TEST_TOKEN_CAT_3_( \
			dcoolImplementationTestExecutor, suiteName_, caseName_ \
		)(::dcool::test::Case::Record& dcoolTestRecord)

#define DCOOL_TEST_BEFORE_FULL_EXECUTION \
		static void dcoolImplementationTestBeforeFullExecution_(); \
		static int dcoolImplementationTestBeforeFullExecutionHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestBeforeFullExecution_, ::dcool::test::Center::instance().beforeFullExecution \
		); \
		static void dcoolImplementationTestBeforeFullExecution_()

#define DCOOL_TEST_AFTER_FULL_EXECUTION \
		static void dcoolImplementationTestAfterFullExecution_(); \
		static int dcoolImplementationTestAfterFullExecutionHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestAfterFullExecution_, ::dcool::test::Center::instance().afterFullExecution \
		); \
		static void dcoolImplementationTestAfterFullExecution_()

#define DCOOL_TEST_BEFORE_EACH_SUITE \
		static void dcoolImplementationTestBeforeSuite_(::dcool::test::Name suiteName); \
		static int dcoolImplementationTestBeforeSuiteHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestBeforeSuite_, ::dcool::test::Center::instance().beforeSuite \
		); \
		static void dcoolImplementationTestBeforeSuite_(::dcool::test::Name suiteName)

#define DCOOL_TEST_AFTER_EACH_SUITE \
		static void dcoolImplementationTestAfterSuite_(::dcool::test::Name suiteName); \
		static int dcoolImplementationTestAfterSuiteHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestAfterSuite_, ::dcool::test::Center::instance().afterSuite \
		); \
		static void dcoolImplementationTestAfterSuite_(::dcool::test::Name suiteName)

#define DCOOL_TEST_BEFORE_EACH_CASE_OF_SUITE(suiteName_) \
		static void dcoolImplementationTestBeforeCase_(::dcool::test::Name caseName); \
		static int dcoolImplementationTestBeforeCaseHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestBeforeCase_, ::dcool::test::Center::instance().forceAccess(#suiteName_).beforeCase \
		); \
		static void dcoolImplementationTestBeforeCase_(::dcool::test::Name caseName)

#define DCOOL_TEST_AFTER_EACH_CASE_OF_SUITE(suiteName_) \
		static void dcoolImplementationTestAfterCase_(::dcool::test::Name caseName); \
		static int dcoolImplementationTestAfterCaseHelper_ = ::dcool::test::detail_::registerListener_( \
			dcoolImplementationTestAfterCase_, ::dcool::test::Center::instance().forceAccess(#suiteName_).afterCase \
		); \
		static void dcoolImplementationTestAfterCase_(::dcool::test::Name caseName)

#endif
