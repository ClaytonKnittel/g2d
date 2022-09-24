
#include "test_utils.h"

#include <inttypes.h>
#include <utils/fun/print_colors.h>

#include <iostream>
#include <memory>

using TestPartResult = ::testing::TestPartResult;
using TestInfo = ::testing::TestInfo;
using TestSuite = ::testing::TestSuite;
using UnitTest = ::testing::UnitTest;

/*
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from LogFixture
[ RUN      ] LogFixture.TestLogNoArguments
[       OK ] LogFixture.TestLogNoArguments (1 ms)
[ RUN      ] LogFixture.TestLogStringArgument
[       OK ] LogFixture.TestLogStringArgument (0 ms)
[ RUN      ] LogFixture.TestLogIntArgument
[       OK ] LogFixture.TestLogIntArgument (0 ms)
[----------] 3 tests from LogFixture (1 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (1 ms total)
[  PASSED  ] 3 tests.
 */

template <typename T>
static std::string CountableStr(T count, const char* singular,
                                const char* plural) {
  std::ostringstream ostr;
  ostr << count << " " << (count == 1 ? singular : plural);
  return ostr.str();
}

ViewedTestListener::ViewedTestListener()
    : cur_suite_(nullptr), cur_info_(nullptr) {}

void ViewedTestListener::OnTestProgramStart(const UnitTest& unit_test) {
  printf("[==========] Running %s from %s.\n",
         CountableStr(unit_test.total_test_count(), "test", "tests").c_str(),
         CountableStr(unit_test.total_test_suite_count(), "test suite",
                      "test suites")
             .c_str());
}

void ViewedTestListener::OnTestIterationStart(const UnitTest& unit_test,
                                              int iteration) {
  if (GTEST_FLAG_GET(repeat) != 1) {
    throw std::runtime_error(
        "ViewedTestListener::OnTestIterationStart unimplemented for repeats > "
        "1");
  }
}

void ViewedTestListener::OnEnvironmentsSetUpStart(const UnitTest& unit_test) {
  printf(P_GREEN "[----------]" P_DEFAULT " Global test environment set-up.\n");
}

void ViewedTestListener::OnEnvironmentsSetUpEnd(const UnitTest& unit_test) {}

void ViewedTestListener::OnTestSuiteStart(const TestSuite& test_suite) {
  printf(P_GREEN "[----------]" P_DEFAULT " %s from %s\n",
         CountableStr(test_suite.test_to_run_count(), "test", "tests").c_str(),
         test_suite.name());

  cur_suite_ = &test_suite;
}

void ViewedTestListener::OnTestStart(const TestInfo& test_info) {
  printf(P_YELLOW "[ RUN      ]" P_DEFAULT " %s.%s\n",
         test_info.test_suite_name(), test_info.name());

  cur_info_ = &test_info;
}

void ViewedTestListener::OnTestDisabled(const TestInfo& test_info) {
  printf("[ DISABLED ] %s.%s\n", test_info.test_suite_name(), test_info.name());
}

static const char* TestPartResultTypeToString(TestPartResult::Type type) {
  switch (type) {
    case TestPartResult::kSkip:
      return "Skipped\n";
    case TestPartResult::kSuccess:
      return "Success";

    case TestPartResult::kNonFatalFailure:
    case TestPartResult::kFatalFailure:
      return "Failure\n";
    default:
      return "Unknown result type";
  }
}

static std::string PrintTestPartResultToString(
    const TestPartResult& test_part_result) {
  std::ostringstream ostr;

  if (test_part_result.file_name() != nullptr) {
    ostr << test_part_result.file_name() << ":";
  }
  if (test_part_result.line_number() >= 0) {
    ostr << test_part_result.line_number() << ":";
  }
  ostr << " " << TestPartResultTypeToString(test_part_result.type())
       << test_part_result.message();

  return ostr.str();
}

void ViewedTestListener::OnTestPartResult(
    const TestPartResult& test_part_result) {
  if (test_part_result.type() == TestPartResult::kSuccess) {
    return;
  }

  res_map_.insert(
      { { cur_suite_->name(), cur_info_->name() }, test_part_result });
  printf("%s\n", PrintTestPartResultToString(test_part_result).c_str());
}

static void PrintFullTestCommentIfPresent(const TestInfo& test_info) {
  const char* const type_param = test_info.type_param();
  const char* const value_param = test_info.value_param();

  if (type_param != nullptr || value_param != nullptr) {
    printf(", where ");
    if (type_param != nullptr) {
      printf("TypeParam = %s", type_param);
      if (value_param != nullptr) {
        printf(" and ");
      }
    }
    if (value_param != nullptr) {
      printf("GetParam() = %s", value_param);
    }
  }
}

void ViewedTestListener::OnTestEnd(const TestInfo& test_info) {
  static const char* passed_str = P_GREEN "[       OK ]" P_DEFAULT " ";
  static const char* skipped_str = P_GREEN "[  SKIPPED ]" P_DEFAULT " ";
  static const char* failed_str = P_RED "[  FAILED  ]" P_DEFAULT " ";

  const char* res_str;
  if (test_info.result()->Passed()) {
    res_str = passed_str;
  } else if (test_info.result()->Passed()) {
    res_str = skipped_str;
  } else {
    res_str = failed_str;
  }

  printf("%s%s.%s", res_str, test_info.test_suite_name(), test_info.name());
  if (test_info.result()->Failed()) {
    PrintFullTestCommentIfPresent(test_info);
  }

  if (GTEST_FLAG_GET(print_time)) {
    printf(" (%" PRId64 " ms)\n", test_info.result()->elapsed_time());
  } else {
    printf("\n");
  }

  cur_info_ = nullptr;
}

void ViewedTestListener::OnTestSuiteEnd(const TestSuite& test_suite) {
  printf("%s[----------]" P_DEFAULT " %s from %s (%" PRId64 " ms total)\n",
         test_suite.failed_test_count() == 0 ? P_GREEN : P_RED,
         CountableStr(test_suite.test_to_run_count(), "test", "tests").c_str(),
         test_suite.name(), test_suite.elapsed_time());

  cur_suite_ = nullptr;
}

void ViewedTestListener::OnEnvironmentsTearDownStart(
    const UnitTest& unit_test) {
  printf(P_GREEN "[----------]" P_DEFAULT
                 " Global test environment tear-down\n");
}

void ViewedTestListener::OnEnvironmentsTearDownEnd(const UnitTest& unit_test) {}

static void PrintFailedTests(const UnitTest& unit_test) {
  int failed_test_count = unit_test.failed_test_count();
  printf(P_RED "[  FAILED  ]" P_DEFAULT " %" PRId32 ", listed below:\n",
         failed_test_count);

  for (int i = 0; i < unit_test.total_test_suite_count(); i++) {
    const TestSuite& test_suite = *unit_test.GetTestSuite(i);
    if (!test_suite.should_run() || (test_suite.failed_test_count() == 0)) {
      continue;
    }
    for (int j = 0; j < test_suite.total_test_count(); j++) {
      const TestInfo& test_info = *test_suite.GetTestInfo(j);
      if (!test_info.should_run() || !test_info.result()->Failed()) {
        continue;
      }
      printf(P_RED "[  FAILED  ]" P_DEFAULT " %s.%s", test_suite.name(),
             test_info.name());
      PrintFullTestCommentIfPresent(test_info);
      printf("\n");
    }
  }
  printf("\n%2d FAILED %s\n", failed_test_count,
         failed_test_count == 1 ? "TEST" : "TESTS");
}

static void PrintFailedTestSuites(const UnitTest& unit_test) {
  int suite_failure_count = 0;
  for (int i = 0; i < unit_test.total_test_suite_count(); i++) {
    const TestSuite& test_suite = *unit_test.GetTestSuite(i);
    if (!test_suite.should_run()) {
      continue;
    }
    if (test_suite.ad_hoc_test_result().Failed()) {
      printf(P_RED "[  FAILED  ]" P_DEFAULT
                   " %s: SetUpTestSuite or TearDownTestSuite\n",
             test_suite.name());
      suite_failure_count++;
    }
  }
  if (suite_failure_count > 0) {
    printf("\n%2d FAILED TEST %s\n", suite_failure_count,
           suite_failure_count == 1 ? "SUITE" : "SUITES");
  }
}

static void PrintSkippedTests(const UnitTest& unit_test) {
  for (int i = 0; i < unit_test.total_test_suite_count(); i++) {
    const TestSuite& test_suite = *unit_test.GetTestSuite(i);
    if (!test_suite.should_run() || (test_suite.skipped_test_count() == 0)) {
      continue;
    }
    for (int j = 0; j < test_suite.total_test_count(); ++j) {
      const TestInfo& test_info = *test_suite.GetTestInfo(j);
      if (!test_info.should_run() || !test_info.result()->Skipped()) {
        continue;
      }
      printf(P_YELLOW "[  SKIPPED ]" P_DEFAULT " %s.%s\n", test_suite.name(),
             test_info.name());
    }
  }
}

void ViewedTestListener::OnTestIterationEnd(const UnitTest& unit_test,
                                            int iteration) {
  printf("[==========] %s from %s ran.",
         CountableStr(unit_test.test_to_run_count(), "test", "tests").c_str(),
         CountableStr(unit_test.test_suite_to_run_count(), "test suite",
                      "test suites")
             .c_str());

  if (GTEST_FLAG_GET(print_time)) {
    printf(" (%" PRId64 " ms total)\n", unit_test.elapsed_time());
  } else {
    printf("\n");
  }

  printf(
      P_GREEN "[  PASSED  ]" P_DEFAULT " %s.\n",
      CountableStr(unit_test.successful_test_count(), "test", "tests").c_str());

  if (unit_test.skipped_test_count() > 0) {
    printf(
        P_YELLOW "[  SKIPPED ]" P_DEFAULT " %s, listed below:\n",
        CountableStr(unit_test.skipped_test_count(), "test", "tests").c_str());
    PrintSkippedTests(unit_test);
  }

  if (!unit_test.Passed()) {
    PrintFailedTests(unit_test);
    PrintFailedTestSuites(unit_test);
  }

  int num_disabled = unit_test.reportable_disabled_test_count();
  if (num_disabled > 0 && !GTEST_FLAG_GET(also_run_disabled_tests)) {
    if (unit_test.Passed()) {
      // Add space if no FAILURE banner displayed.
      printf("\n");
    }
    printf(P_LGRAY "  YOU HAVE %d DISABLED %s\n\n", num_disabled,
           num_disabled == 1 ? "TEST" : "TESTS");
  }
}

void ViewedTestListener::OnTestProgramEnd(const UnitTest& unit_test) {}
