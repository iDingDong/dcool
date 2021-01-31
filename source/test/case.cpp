#include <dcool/test/case.hpp>

namespace dcool::test {
	namespace {
		class FatalCaseFailure: public ::std::runtime_error {
			private: using Super_ = ::std::runtime_error;

			public: using Super_::Super_;
		};
	}

	Case::Case(Case::Executor executor) noexcept: m_executor_(executor) {
	}

	auto Case::execute() const -> Case::Result {
		Result result = {};
		result.startTime = Clock::now();
		try {
			this->m_executor_(result.record);
			result.endedByFatal = false;
		} catch (FatalCaseFailure const&) {
			result.endedByFatal = true;
		}
		result.finishTime = Clock::now();
		return result;
	}

	namespace detail_ {
		void check_(
			FileName fileName, LineNumber lineNumber, Case::Failure::Level level, core::Boolean predicate, Case::Record& record
		) {
			++(record.checkCount);
			if (!predicate) {
				record.failures.emplaceBack(Case::Failure { .fileName = fileName, .lineNumber = lineNumber, .time = Clock::now() });
				if (level == Case::Failure::Level::fatalForCase) {
					throw FatalCaseFailure("Case encoutered fatal failure.");
				}
			}
		}
	}
}
