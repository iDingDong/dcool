#include <dcool/basic.hpp>
#include <dcool/console.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <iostream>
#include <string>

// Remove this workaround when 'operator <<(std::ostream, std::chrono::duration)' is implemented by libstdc++.
static auto executionDurationString(dcool::test::Duration const& duration) -> std::string {
	return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) + "us";
}

static auto writeSuiteName(std::ostream& stream, dcool::test::Name suiteName) {
	dcool::console::applyForegroundColorSgr(stream, dcool::console::cyan8);
	stream << suiteName;
	dcool::console::applySgrReset(stream);
}

static auto writeAbsoluteCaseName(std::ostream& stream, dcool::test::Name suiteName, dcool::test::Name caseName) {
	writeSuiteName(stream, suiteName);
	stream << '.';
	dcool::console::applyForegroundColorSgr(stream, dcool::console::yellow8);
	stream << caseName;
	dcool::console::applySgrReset(stream);
}

int main() {
	dcool::console::applySgrReset(std::cout);
	std::cout << dcool::commonSelfNativeDescription << std::endl;
	auto result = dcool::test::Center::instance().executeAll(dcool::core::parallelExecution, dcool::core::parallelExecution);
	std::cout << "Test report:\n";
	for (auto& suiteResult: result.details) {
		std::cout << "\t[Suite ";
		writeSuiteName(std::cout, suiteResult.first);
		std::cout << "] detail:\n";
		for (auto& caseResult: suiteResult.second.details) {
			std::cout << "\t\t[Case ";
			writeAbsoluteCaseName(std::cout, suiteResult.first, caseResult.first);
			std::cout << "] detail:\n";
			for (auto& failure: caseResult.second.record.failures) {
				dcool::console::applyForegroundColorSgr(std::cout, dcool::console::red8);
				std::cout <<
					"\t\t\tCheck did not pass at '" <<
					failure.location.file_name() <<
					":" <<
					failure.location.line() <<
					'.'
				;
				dcool::console::applySgrReset(std::cout);
				std::cout << '\n';
			}
			std::cout << "\t\t[Case ";
			writeAbsoluteCaseName(std::cout, suiteResult.first, caseResult.first);
			std::cout << "] ";
			if (caseResult.second.record.failures.length() > 0) {
				dcool::console::applyForegroundColorSgr(std::cout, dcool::console::red8);
				std::cout <<
					caseResult.second.record.failures.length() <<
					" of " << caseResult.second.record.checkCount <<
					" performed check(s) failed " <<
					(caseResult.second.endedByFatal ? "with last failure fatal" : "gently")
				;
			} else {
				dcool::console::applyForegroundColorSgr(std::cout, dcool::console::green8);
				std::cout << "All " << caseResult.second.record.checkCount << " performed checks passed";
			}
			std::cout << " in " << executionDurationString(caseResult.second.finishTime - caseResult.second.startTime) << '.';
			dcool::console::applySgrReset(std::cout);
			std::cout << '\n';
		}
		std::cout << "\t[Suite ";
		writeSuiteName(std::cout, suiteResult.first);
		std::cout << "] ";
		dcool::test::Count failureCount = suiteResult.second.gentleFailureCount + suiteResult.second.fatalFailureCount;
		dcool::test::Count caseCount = suiteResult.second.successCount + failureCount;
		if (failureCount > 0) {
			dcool::console::applyForegroundColorSgr(std::cout, dcool::console::red8);
			std::cout <<
				failureCount <<
				"(" <<
				suiteResult.second.fatalFailureCount <<
				" fatal) of " <<
				caseCount <<
				" cases executed failed"
			;
		} else {
			dcool::console::applyForegroundColorSgr(std::cout, dcool::console::green8);
			std::cout << "All " << caseCount << " cases passed";
		}
		std::cout << " in " << executionDurationString(suiteResult.second.finishTime - suiteResult.second.startTime) << '.';
		dcool::console::applySgrReset(std::cout);
		std::cout << '\n';
	}
	dcool::test::Count suiteCount = result.suiteStatistics.successCount + result.suiteStatistics.failureCount;
	if (result.suiteStatistics.failureCount > 0) {
		dcool::console::applyForegroundColorSgr(std::cout, dcool::console::red8);
		std::cout << result.suiteStatistics.failureCount << " of " << suiteCount << " suites executed failed";
	} else {
		dcool::console::applyForegroundColorSgr(std::cout, dcool::console::green8);
		std::cout << "All " << suiteCount << " suites passed";
	}
	std::cout << " in " << executionDurationString(result.finishTime - result.startTime) << '.';
	dcool::console::applySgrReset(std::cout);
	std::cout << std::endl;
	return static_cast<int>(result.suiteStatistics.failureCount);
}
