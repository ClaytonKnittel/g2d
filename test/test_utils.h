#ifndef _TEST_UTILS_H
#define _TEST_UTILS_H

#include <gtest/gtest.h>

#include <memory>
#include <unordered_map>

// Maps test suite/name to
typedef std::pair<std::string, std::string> TestKey;

class TestKeyHash {
 public:
  size_t operator()(const TestKey& key) const {
    std::hash<std::string> str_hash;
    return str_hash(key.first) ^ str_hash(key.second);
  }
};

typedef std::unordered_map<TestKey, ::testing::TestPartResult, TestKeyHash>
    TestResultMap;

class ViewedTestListener : public ::testing::TestEventListener {
  using TestInfo = ::testing::TestInfo;
  using TestPartResult = ::testing::TestPartResult;
  using TestSuite = ::testing::TestSuite;
  using UnitTest = ::testing::UnitTest;

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
  TestResultMap res_map_;
  const TestSuite* cur_suite_;
  const TestInfo* cur_info_;
};

#endif /* _TEST_UTILS_H */
