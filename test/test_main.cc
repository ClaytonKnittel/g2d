
#include <gtest/gtest.h>

#include "test_utils.h"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  // Remove the default test event listener
  ::testing::TestEventListeners& event_listeners =
      ::testing::UnitTest::GetInstance()->listeners();
  event_listeners.Release(event_listeners.default_result_printer());

  ViewedTestListener* listener = new ViewedTestListener();
  event_listeners.Append(listener);

  return RUN_ALL_TESTS();
}
