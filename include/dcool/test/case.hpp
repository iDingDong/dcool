#ifndef DCOOL_TEST_CASE_HPP_INCLUDED_
#	define DCOOL_TEST_CASE_HPP_INCLUDED_ 1

#	include <dcool/test/basic.hpp>

#	include <dcool/container.hpp>

#	include <stdexcept>

namespace dcool::test {


	class Case {
		private: using Self_ = Case;

		public: struct Failure {
			enum class Level {
				gentle,
				fatalForCase
			};

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
			::dcool::test::TimePoint endTime;
			::dcool::core::Boolean endedByFatal;
			Record record;
		};

		public: using Executor = void(*)(Record& dcoolTestRecord_);

		private: Executor m_executor_;

		public: explicit Case(Executor executor_) noexcept;
		public: auto execute() -> Result;
	};

	namespace detail_ {
		void check_(
			::dcool::test::FileName fileName_,
			::dcool::test::LineNumber lineNumber_,
			::dcool::test::Case::Failure::Level level_,
			::dcool::core::Boolean predicate_,
			::dcool::test::Case::Record& record_
		);
	}
}

#	define DCOOL_TEST_CHECK(level_, predicate_) \
	::dcool::test::detail_::check_(__FILE__, __LINE__, (level_), (predicate_), dcoolTestRecord)

#	define DCOOL_TEST_EXPECT(predicate_) DCOOL_TEST_CHECK(::dcool::test::Case::Failure::Level::gentle, (predicate_))

#	define DCOOL_TEST_ASSERT(predicate_) DCOOL_TEST_CHECK(::dcool::test::Case::Failure::Level::fatalForCase, (predicate_))

#endif
