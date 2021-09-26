#include <dcool/basic.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <iostream>
#include <string>

// Remove this workaround when 'operator <<(std::ostream, std::chrono::duration)' is implemented by libstdc++.
static auto executionDurationString(dcool::test::Duration const& duration) -> std::string {
	return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) + "us";
}

int main() {
	std::cout << dcool::commonSelfNativeDescription << std::endl;
	auto result = dcool::test::Center::instance().executeAll(dcool::core::parallelExecution, dcool::core::parallelExecution);
	std::cout << "Test report:\n";
	for (auto& suiteResult: result.details) {
		std::cout << "\t[Suite " << suiteResult.first << "] detail:\n";
		for (auto& caseResult: suiteResult.second.details) {
			std::cout << "\t\t[Case " << suiteResult.first << "." << caseResult.first << "] detail:\n";
			for (auto& failure: caseResult.second.record.failures) {
				std::cout << "\t\t\tCheck did not pass at '" << failure.fileName << ":" << failure.lineNumber << "\n";
			}
			std::cout << "\t\t[Case " << suiteResult.first << "." << caseResult.first << "] ";
			if (caseResult.second.record.failures.length() > 0) {
				std::cout <<
					caseResult.second.record.failures.length() <<
					" of " << caseResult.second.record.checkCount <<
					" performed check(s) failed " <<
					(caseResult.second.endedByFatal ? "with last failure fatal" : "gently")
				;
			} else {
				std::cout  << "All " << caseResult.second.record.checkCount << " performed checks passed";
			}
			std::cout << " in " << executionDurationString(caseResult.second.finishTime - caseResult.second.startTime) << ".\n";
		}
		std::cout << "\t[Suite " << suiteResult.first << "] ";
		dcool::test::Count failureCount = suiteResult.second.gentleFailureCount + suiteResult.second.fatalFailureCount;
		dcool::test::Count caseCount = suiteResult.second.successCount + failureCount;
		if (failureCount > 0) {
			std:: cout <<
				failureCount <<
				"(" <<
				suiteResult.second.fatalFailureCount <<
				" fatal) of " <<
				caseCount <<
				" cases executed failed"
			;
		} else {
			std::cout << "All " << caseCount << " cases passed";
		}
		std::cout << " in " << executionDurationString(suiteResult.second.finishTime - suiteResult.second.startTime) << ".\n";
	}
	dcool::test::Count suiteCount = result.suiteStatistics.successCount + result.suiteStatistics.failureCount;
	if (result.suiteStatistics.failureCount > 0) {
		std::cout <<
			result.suiteStatistics.failureCount <<
			" of " <<
			suiteCount <<
			" suites executed failed"
		;
	} else {
		std::cout << "All " << suiteCount << " suites passed";
	}
	std::cout << " in " << executionDurationString(result.finishTime - result.startTime) << std::endl;
	return static_cast<int>(result.suiteStatistics.failureCount);
}
