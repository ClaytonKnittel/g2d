#ifndef _TEST_UTILS_H
#define _TEST_UTILS_H

#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <unordered_map>

// Maps test suite/name to
typedef std::pair<std::string, std::string> TestKey;

class TestKeyHash {
 public:
  size_t operator()(const TestKey& key) const {
    std::hash<std::string> str_hash;
    size_t h1 = str_hash(key.first);
    size_t h2 = str_hash(key.second);
    return h1 ^ ((h2 >> 32) | (h2 << 32));
  }
};

typedef std::unordered_multimap<TestKey, ::testing::TestPartResult, TestKeyHash>
    TestResultMap;

enum class TestState {
  RUNNING,
  FINISHED,
};

class ViewedTestListener : public ::testing::TestEventListener {
  using TestInfo = ::testing::TestInfo;
  using TestPartResult = ::testing::TestPartResult;
  using TestSuite = ::testing::TestSuite;
  using UnitTest = ::testing::UnitTest;

 private:
  void PrintFailedTests(const UnitTest& unit_test);
  void PrintFailedTestSuites(const UnitTest& unit_test);
  void PrintSkippedTests(const UnitTest& unit_test);

  void RefreshTestMonitor();
  void InitTestMonitor();
  void CloseTestMonitor();

  void PrintTestBegin(const TestInfo& info);
  void PrintTestFinish();

 public:
  ViewedTestListener();

  virtual void OnTestProgramStart(const UnitTest& unit_test);
  virtual void OnTestIterationStart(const UnitTest& unit_test, int iteration);
  virtual void OnEnvironmentsSetUpStart(const UnitTest& unit_test);
  virtual void OnEnvironmentsSetUpEnd(const UnitTest& unit_test);
  virtual void OnTestSuiteStart(const TestSuite& test_suite);
  virtual void OnTestStart(const TestInfo& test_info);
  virtual void OnTestDisabled(const TestInfo& test_info);
  virtual void OnTestPartResult(const TestPartResult& test_part_result);
  virtual void OnTestEnd(const TestInfo& test_info);
  virtual void OnTestSuiteEnd(const TestSuite& test_suite);
  virtual void OnEnvironmentsTearDownStart(const UnitTest& unit_test);
  virtual void OnEnvironmentsTearDownEnd(const UnitTest& unit_test);
  virtual void OnTestIterationEnd(const UnitTest& unit_test, int iteration);
  virtual void OnTestProgramEnd(const UnitTest& unit_test);

 private:
  static constexpr uint32_t default_n_test_lines_ = 5;

  TestResultMap res_map_;
  const UnitTest* cur_unit_;
  const TestSuite* cur_suite_;
  const TestInfo* cur_info_;

  // Number of test lines to show at a time (not including header lines)
  const uint32_t n_test_lines_;
  // Number of currently allocated test lines (not including header lines)
  uint32_t n_allocated_test_lines_;
  uint32_t n_tests_run_;

  std::deque<std::pair<const TestInfo*, TestState>> test_result_lines_;
};

#endif /* _TEST_UTILS_H */
