#ifndef DCOOL_TEST_CASE_HPP_INCLUDED_
#	define DCOOL_TEST_CASE_HPP_INCLUDED_ 1

#	include <dcool/test/basic.hpp>

#	include <dcool/core.hpp>
#	include <dcool/container.hpp>

#	include <atomic>
#	include <algorithm>
#	include <iterator>
#	include <source_location>

namespace dcool::test {
	class Case {
		private: using Self_ = Case;

		public: struct Failure {
			enum class Level {
				gentle,
				fatalForCase
			};

			::std::source_location location;
			::dcool::test::TimePoint time;
		};

		public: struct Record {
			::dcool::test::Count checkCount = 0;
			::dcool::container::List<Failure> failures;
		};

		public: struct ActiveRecord {
			private: ::std::atomic<::dcool::test::Count> m_checkCount_;
			private: ::dcool::container::List<Failure> m_failures_;
			private: mutable ::dcool::core::MinimalMutex m_mutex_;

			public: ActiveRecord() noexcept;
			public: ActiveRecord(ActiveRecord const& other_) = delete;
			// public: ActiveRecord(Record record_);
			public: auto operator =(ActiveRecord const& other_) -> ActiveRecord& = delete;
			// public: auto operator =(Record record_) -> ActiveRecord&;
			public: ~ActiveRecord() noexcept;

			public: auto snapshot() const -> Record;
			public: void recordSuccess();
			public: void recordFailure(Failure failure_);
		};

		public: struct Context {
			::dcool::test::Name const& suiteName;
			::dcool::test::Name const& caseName;
			ActiveRecord activeRecord;
		};

		public: struct Result {
			::dcool::test::TimePoint startTime;
			::dcool::test::TimePoint finishTime;
			::dcool::core::Boolean endedByFatal;
			Record record;
		};

		public: using Executor = void(*)(Context& context_);

		private: Executor m_executor_;

		public: explicit Case(Executor executor_) noexcept;
		public: auto execute(::dcool::test::Name suiteName_, ::dcool::test::Name caseName_) const -> Result;
	};

	namespace detail_ {
		void check_(
			::dcool::test::Case::Context& context_,
			::dcool::test::Case::Failure::Level level_,
			::dcool::core::Boolean predicate_,
			const ::std::source_location& location_ = ::std::source_location::current()
		);

		template <typename LeftRangeT_, typename RightRangeT_> void checkRangeEquality_(
			::dcool::test::Case::Context& context_,
			::dcool::test::Case::Failure::Level level_,
			LeftRangeT_&& left_,
			RightRangeT_&& right_,
			const ::std::source_location& location_ = ::std::source_location::current()
		) {
			// GCC 10.2.0 introduced an ICE if 'std::ranges::equal' is used.
			::dcool::test::detail_::check_(
				context_,
				level_,
				::std::equal(::std::begin(left_), ::std::end(left_), ::std::begin(right_), ::std::end(right_)),
				location_
			);
		}
	}
}

#endif
