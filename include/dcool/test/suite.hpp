#ifndef DCOOL_TEST_SUITE_HPP_INCLUDED_
#	define DCOOL_TEST_SUITE_HPP_INCLUDED_ 1

#	include <dcool/test/basic.hpp>
#	include <dcool/test/case.hpp>

#	include <dcool/container.hpp>

#	include <map>

#	include <stdexcept>

namespace dcool::test {
	class CaseNotFound: public ::std::out_of_range {
		private: using Super_ = ::std::out_of_range;

		public: using Super_::Super_;
	};

	class Suite {
		private: using Self_ = Suite;

		public: struct Result {
			::dcool::test::Count successCount;
			::dcool::test::Count gentleFailureCount;
			::dcool::test::Count fatalFailureCount;
			::std::map<::dcool::test::Name, ::dcool::test::Case::Result> details;
		};

		private: std::map<::dcool::test::Name, ::dcool::test::Case> m_cases_;

		public: void addCase(::dcool::test::Name caseName_, ::dcool::test::Case::Executor executor_);
		public: auto access(::dcool::test::Name caseName) -> ::dcool::test::Case*;
		public: auto executeAll() -> Result;
		public: auto executeOne(::dcool::test::Name caseName_) -> ::dcool::test::Case::Result;
	};
}

#endif
