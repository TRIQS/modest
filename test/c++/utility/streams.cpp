#include <gtest/gtest.h>
#include <triqs/utility/streams.hpp>

using namespace triqs;

TEST(StreamWithVerbosityTest, WritesWhenVerbosityPositive) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 1);
  stream << "Hello World!";
  EXPECT_EQ(output.str(), "Hello World!");
}

TEST(StreamWithVerbosityTest, SkipsWriteWhenVerbosityZero) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 0);
  stream << "This should not appear.";
  EXPECT_EQ(output.str(), "");
}

TEST(StreamWithVerbosityTest, WritesMultipleValues) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 2);
  stream << "Value: " << 42 << ", Pi: " << 3.14;
  EXPECT_EQ(output.str(), "Value: 42, Pi: 3.14");
}

TEST(StreamWithVerbosityTest, AppliesEndlCorrectly) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 1);
  stream << "Line1" << std::endl << "Line2";
  EXPECT_EQ(output.str(), "Line1\nLine2");
}

TEST(StreamWithVerbosityTest, OperatorCallReducesVerbosity) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 3);
  auto lower_verbosity = stream(2); // Decreases verbosity by (2-1) = 1
  lower_verbosity << "Visible";
  EXPECT_EQ(output.str(), "Visible");
}

TEST(StreamWithVerbosityTest, NestedVerbosityDropsBelowZero) {
  std::ostringstream output;
  ostream_with_verbosity stream(output, 1);
  auto silent = stream(3); // Verbosity becomes negative
  silent << "This should not be printed.";
  EXPECT_EQ(output.str(), "");
}
