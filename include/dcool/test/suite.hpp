#ifndef DCOOL_TEST_SUITE_HPP_INCLUDED_
#	define DCOOL_TEST_SUITE_HPP_INCLUDED_ 1

#	include <dcool/test/basic.hpp>
#	include <dcool/test/case.hpp>

#	include <dcool/algorithm.hpp>
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
			::dcool::test::TimePoint startTime;
			::dcool::test::TimePoint finishTime;
			::std::map<::dcool::test::Name, ::dcool::test::Case::Result> details;
		};

		public: using BeforeCase = void(*)(::dcool::test::Name);
		public: using AfterCase = void(*)(::dcool::test::Name);

		public: BeforeCase beforeCase = ::dcool::core::nullPointer;
		public: AfterCase afterCase = ::dcool::core::nullPointer;
		private: ::std::map<::dcool::test::Name, ::dcool::test::Case> m_cases_;

		private: void setUpBeforeCase_(::dcool::test::Name caseName_) const;
		private: void tearDownAfterCase_(::dcool::test::Name caseName_) const;
		public: void addCase(::dcool::test::Name caseName_, ::dcool::test::Case::Executor executor_);
		public: auto getPointer(::dcool::test::Name caseName) const -> ::dcool::test::Case const*;
		public: auto getPointer(::dcool::test::Name caseName) -> ::dcool::test::Case*;

		public: template <::dcool::core::FormOfExecutionPolicy PolicyT_> auto executeAll(
			PolicyT_&& policy_, ::dcool::test::Name suiteName_
		) const -> Result {
			Result result_ = {};
			for (auto const& case_: this->m_cases_) {
				result_.details.insert(::std::make_pair(case_.first, Case::Result {}));
			}
			result_.startTime = Clock::now();
			::dcool::algorithm::forEach(
				::dcool::core::forward<PolicyT_>(policy_),
				this->m_cases_.begin(),
				this->m_cases_.end(),
				[this, &suiteName_, &result_](auto& case_) {
					this->setUpBeforeCase_(case_.first);
					::dcool::test::Case::Result caseResult_ = case_.second.execute(suiteName_, case_.first);
					this->tearDownAfterCase_(case_.first);
					::dcool::core::dereference(result_.details.find(case_.first)).second = caseResult_;
				}
			);
			for (auto const& caseResult_: result_.details) {
				if (caseResult_.second.record.failures.vacant()) {
					++(result_.successCount);
				} else if (caseResult_.second.endedByFatal) {
					++(result_.fatalFailureCount);
				} else {
					++(result_.gentleFailureCount);
				}
			}
			result_.finishTime = Clock::now();
			return result_;
		}

		public: auto executeAll(::dcool::test::Name suiteName_) const -> Result;
		public: auto executeOne(::dcool::test::Name suiteName_, ::dcool::test::Name caseName_) const -> ::dcool::test::Case::Result;
	};
}

#endif
