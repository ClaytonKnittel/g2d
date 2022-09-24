
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/os/dircntl.h>

#include <g2d/common/log.h>

using ::testing::MatchesRegex;

class LogFixture : public ::testing::Test {
 private:
  void _InitTmpDir() {
    ASSERT_EQ(mkdirp(tmp_dir_, S_IRWXU), 0);
  }

 protected:
  void SetUp() override {
    _InitTmpDir();

    char fname[] = "/tmp/g2d_unit_tests/test_log/XXXXXX";
    tmp_fd_ = mkstemp(fname);
    ASSERT_GE(tmp_fd_, 0);

    // redirect stderr
    stderr_fd_ = dup(STDERR_FILENO);
    dup2(tmp_fd_, STDERR_FILENO);

    last_offset_ = 0;
  }

  void TearDown() override {
    close(tmp_fd_);
    dup2(stderr_fd_, STDERR_FILENO);
  }

 public:
  std::string ReadStderr() {
    std::ostringstream str_builder;
    char buf[1024];
    ssize_t n_chars;

    lseek(tmp_fd_, last_offset_, SEEK_SET);

    do {
      n_chars = read(tmp_fd_, buf, sizeof(buf));
      if (n_chars < 0) {
        break;
      }

      str_builder.write(buf, n_chars);
      last_offset_ += n_chars;
    } while (n_chars > 0);

    return str_builder.str();
  }

  static std::string GenLogRegex(const std::string& log_lvl,
                                 const std::string& message) {
    return (std::ostringstream()
            << "[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2} UTC \\["
            << log_lvl << "\\] " << message << "\n")
        .str();
  }

 private:
  static const char* tmp_dir_;
  int stderr_fd_;
  int tmp_fd_;
  off_t last_offset_;
};

const char* LogFixture::tmp_dir_ = "/tmp/g2d_unit_tests/test_log";

TEST_F(LogFixture, TestLogNoArguments) {
  g2dlog(INF, "Hello");
  EXPECT_THAT(ReadStderr(), MatchesRegex(GenLogRegex("INF", "Hello")));
}

TEST_F(LogFixture, TestLogStringArgument) {
  g2dlog(INF, "Hello, %s", "World");
  EXPECT_THAT(ReadStderr(), MatchesRegex(GenLogRegex("INF", "Hello, World")));
}

TEST_F(LogFixture, TestLogIntArgument) {
  g2dlog(INF, "Hello, %d", 5);
  EXPECT_THAT(ReadStderr(), MatchesRegex(GenLogRegex("INF", "Hello, 5")));
}
