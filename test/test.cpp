#include <dcool/test.hpp>

#include <iostream>

int main() {
	auto result = dcool::test::Center::instance().executeAll();
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
					" performed checke(s) failed with " <<
					(caseResult.second.endedByFatal ? "last failure fatal" : "all failures gentle") <<
					".\n"
				;
			} else {
				std::cout  << "All " << caseResult.second.record.checkCount << " performed checks passed.\n";
			}
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
				" cases executed failed.\n"
			;
		} else {
			std::cout << "All " << caseCount << " cases passed.\n";
		}
	}
	dcool::test::Count suiteCount = result.suiteStatistics.successCount + result.suiteStatistics.failureCount;
	if (result.suiteStatistics.failureCount > 0) {
		std::cout <<
			result.suiteStatistics.failureCount <<
			" of " <<
			suiteCount <<
			" suites executed failed." <<
			std::endl
		;
	} else {
		std::cout << "All " << suiteCount << " suites passed.\n";
	}
	return 0;
}
