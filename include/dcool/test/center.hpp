#ifndef DCOOL_TEST_CENTER_HPP_INCLUDED_
#	define DCOOL_TEST_CENTER_HPP_INCLUDED_ 1

#	include <dcool/test/suite.hpp>

#	include <map>
#	include <string_view>

namespace dcool::test {
	class SuiteNotFound: public ::std::out_of_range {
		private: using Super_ = ::std::out_of_range;

		public: using Super_::Super_;
	};

	class Center {
		private: using Self_ = Center;

		private: std::map<::dcool::test::Name, ::dcool::test::Suite> m_suites_;

		public: struct Result {
			struct {
				::dcool::test::Count successCount;
				::dcool::test::Count failureCount;
			} suiteStatistics;
			struct {
				::dcool::test::Count successCount;
				::dcool::test::Count gentleFailureCount;
				::dcool::test::Count fatalFailureCount;
			} caseStatistics;
			::std::map<::dcool::test::Name, ::dcool::test::Suite::Result> details;
		};

		public: void addSuite(::dcool::test::Name suiteName_);
		public: auto access(::dcool::test::Name suiteName_) -> ::dcool::test::Suite*;
		public: auto executeAll() -> Result;
		public: auto executeOne(::dcool::test::Name suiteName_) -> ::dcool::test::Suite::Result;
		public: static auto instance() -> Self_&;
	};

	namespace detail_ {
		int registerCase_(::dcool::test::Name suiteName_, ::dcool::test::Name caseName_, ::dcool::test::Case::Executor executor_);
	}
}


#define DCOOL_TEST_TOKEN_CAT_3_HELPER_(leading_, middle_, following_) leading_##middle_##following_
#define DCOOL_TEST_TOKEN_CAT_3_(leading_, middle_, following_) DCOOL_TEST_TOKEN_CAT_3_HELPER_(leading_, middle_, following_)

#define DCOOL_TEST_CASE(suiteName_, caseName_) \
	void DCOOL_TEST_TOKEN_CAT_3_(dcoolTestExecutor_, suiteName_, caseName_)(::dcool::test::Case::Record& dcoolTestRecord); \
	static int DCOOL_TEST_TOKEN_CAT_3_(g_dcoolTestCaseHelper_, suiteName_, caseName_) = ::dcool::test::detail_::registerCase_( \
		#suiteName_, #caseName_, DCOOL_TEST_TOKEN_CAT_3_(dcoolTestExecutor_, suiteName_, caseName_) \
	); \
	void DCOOL_TEST_TOKEN_CAT_3_(dcoolTestExecutor_, suiteName_, caseName_)(::dcool::test::Case::Record& dcoolTestRecord)

#endif
