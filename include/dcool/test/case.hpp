#ifndef DCOOL_TEST_CASE_HPP_INCLUDED_
#	define DCOOL_TEST_CASE_HPP_INCLUDED_ 1

#	include <dcool/test/basic.hpp>

#	include <dcool/container.hpp>

#	include <algorithm>
#	include <stdexcept>
#	include <initializer_list>

namespace dcool::test {
	class Case {
		private: using Self_ = Case;

		public: struct Failure {
			enum class Level {
				gentle,
				fatalForCase
			};

			// Use std::source_location instead once it is available. Effectively we are waiting for GCC 11 for the intrinsics.
			::dcool::test::FileName fileName;
			::dcool::test::LineNumber lineNumber;
			::dcool::test::TimePoint time;
		};

		public: struct Record {
			::dcool::test::Count checkCount;
			::dcool::container::List<Failure> failures;
		};

		public: struct Result {
			::dcool::test::TimePoint startTime;
			::dcool::test::TimePoint finishTime;
			::dcool::core::Boolean endedByFatal;
			Record record;
		};

		public: using Executor = void(*)(Record& dcoolTestRecord_);

		private: Executor m_executor_;

		public: explicit Case(Executor executor_) noexcept;
		public: auto execute() const -> Result;
	};

	namespace detail_ {
		void check_(
			::dcool::test::FileName fileName_,
			::dcool::test::LineNumber lineNumber_,
			::dcool::test::Case::Failure::Level level_,
			::dcool::core::Boolean predicate_,
			::dcool::test::Case::Record& record_
		);

		template <::std::ranges::input_range LeftRangeT_, ::std::ranges::input_range RightRangeT_> void checkRangeEquality_(
			::dcool::test::FileName fileName_,
			::dcool::test::LineNumber lineNumber_,
			::dcool::test::Case::Failure::Level level_,
			LeftRangeT_&& left_,
			RightRangeT_&& right_,
			::dcool::test::Case::Record& record_
		) {
			::dcool::test::detail_::check_(
				fileName_,
				lineNumber_,
				level_,
				::std::ranges::equal(::dcool::core::forward<LeftRangeT_>(left_), ::dcool::core::forward<RightRangeT_>(right_)),
				record_
			);
		}
	}
}

#endif
