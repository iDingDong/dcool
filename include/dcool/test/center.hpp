#ifndef DCOOL_TEST_CENTER_HPP_INCLUDED_
#	define DCOOL_TEST_CENTER_HPP_INCLUDED_ 1

#	include <dcool/test/suite.hpp>

#	include <dcool/algorithm.hpp>
#	include <dcool/core.hpp>

#	include <map>
#	include <string_view>

namespace dcool::test {
	class SuiteNotFound: public ::std::out_of_range {
		private: using Super_ = ::std::out_of_range;

		public: using Super_::Super_;
	};

	class Center {
		private: using Self_ = Center;

		public: using BeforeFullExecution = void(*)();
		public: using AfterFullExecution = void(*)();
		public: using BeforeSuite = void(*)(::dcool::test::Name);
		public: using AfterSuite = void(*)(::dcool::test::Name);

		public: BeforeFullExecution beforeFullExecution = ::dcool::core::nullPointer;
		public: AfterFullExecution afterFullExecution = ::dcool::core::nullPointer;
		public: BeforeSuite beforeSuite = ::dcool::core::nullPointer;
		public: AfterSuite afterSuite = ::dcool::core::nullPointer;
		private: ::std::map<::dcool::test::Name, ::dcool::test::Suite> m_suites_;

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
			::dcool::test::TimePoint startTime;
			::dcool::test::TimePoint finishTime;
			::std::map<::dcool::test::Name, ::dcool::test::Suite::Result> details;
		};

		private: void setUpBeforeFullExecution_() const;
		private: void tearDownAfterFullExecution_() const;
		private: void setUpBeforeSuite_(::dcool::test::Name suiteName_) const;
		private: void tearDownAfterSuite_(::dcool::test::Name suiteName_) const;
		public: void addSuite(::dcool::test::Name suiteName_);
		public: auto getPointer(::dcool::test::Name suiteName_) const -> ::dcool::test::Suite const*;
		public: auto getPointer(::dcool::test::Name suiteName_) -> ::dcool::test::Suite*;
		public: auto forceAccess(::dcool::test::Name suiteName_) -> ::dcool::test::Suite&;

		public: template <
			::dcool::core::FormOfExecutionPolicy SuitePolicyT_, ::dcool::core::FormOfExecutionPolicy CasePolicyT_
		> auto executeAll(SuitePolicyT_&& suitePolicy_, CasePolicyT_&& casePolicy_) const -> Result {
			Result result_ = {};
			for (auto const& suite_: this->m_suites_) {
				result_.details.insert(::std::make_pair(suite_.first, Suite::Result{}));
			}
			result_.startTime = Clock::now();
			this->setUpBeforeFullExecution_();
			::dcool::algorithm::forEach(
				::dcool::core::forward<SuitePolicyT_>(suitePolicy_),
				this->m_suites_.begin(),
				this->m_suites_.end(),
				[this, &result_, &casePolicy_](auto& suite_) {
					this->setUpBeforeSuite_(suite_.first);
					::dcool::test::Suite::Result suiteResult_ = suite_.second.executeAll(casePolicy_);
					this->tearDownAfterSuite_(suite_.first);
					::dcool::core::dereference(result_.details.find(suite_.first)).second = suiteResult_;
				}
			);
			for (auto const& suiteResult_: result_.details) {
				if (suiteResult_.second.fatalFailureCount != 0 || suiteResult_.second.gentleFailureCount != 0) {
					++(result_.suiteStatistics.failureCount);
				} else {
					++(result_.suiteStatistics.successCount);
				}
				result_.caseStatistics.successCount += suiteResult_.second.successCount;
				result_.caseStatistics.gentleFailureCount += suiteResult_.second.gentleFailureCount;
				result_.caseStatistics.fatalFailureCount += suiteResult_.second.fatalFailureCount;
			}
			this->tearDownAfterFullExecution_();
			result_.finishTime = Clock::now();
			return result_;
		}

		public: template <
			::dcool::core::FormOfExecutionPolicy SuitePolicyT_
		> auto executeAll(SuitePolicyT_&& suitePolicy_) const -> Result {
			return this->executeAll(::dcool::core::forward<SuitePolicyT_>(suitePolicy_), ::dcool::core::serialExecution);
		}

		public: auto executeAll() const -> Result;
		public: auto executeOne(::dcool::test::Name suiteName_) const -> ::dcool::test::Suite::Result;
		public: static auto instance() -> Self_&;
	};

	namespace detail_ {
		int registerCase_(::dcool::test::Name suiteName_, ::dcool::test::Name caseName_, ::dcool::test::Case::Executor executor_);

		template <typename ListenerT_, typename DestinationT_> int registerListener_(
			ListenerT_&& listener_, DestinationT_& trigger_
		) {
			if (trigger_ != ::dcool::core::nullPointer) {
				::dcool::core::terminate();
			}
			trigger_ = ::dcool::core::forward<ListenerT_>(listener_);
			return 0;
		}
	}
}

#endif
