#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

#include "impl/PlainTextMatchCollector.h"

using namespace gui;

TEST(PlainTextMatchCollectorTest, failure) {
   // Create tested class
  PlainTextMatchCollector collector;
  // Connect spy to the signal to collect if something is emitted
  QSignalSpy spy(&collector, &PlainTextMatchCollector::append);
  // Call the function and expect that signal will be emitted
  collector.failed("test/my-file.txt");
  // Check the number of signals generated
  ASSERT_EQ(spy.count(), 1);
  // Obtain parameters for the first generated signal
  auto arguments = spy.takeFirst();
  // Compare result for the first signal with expected value
  EXPECT_EQ(arguments.at(0).toString(), "Filed to parse: test/my-file.txt");
}

TEST(PlainTextMatchCollectorTest, startFileAndMatch) {
  // Construct tested class
  PlainTextMatchCollector collector;
  // Connect `spy` to the signal to record any generated signal
  QSignalSpy spy(&collector, &PlainTextMatchCollector::append);
  // Check the positive scenario, when program starts check for a file and found
  // one match
  collector.startFile("test/my-file.txt");
  collector.matchedLine("test line");

  // Check that two signals are generated
  ASSERT_EQ(spy.count(), 2);

  // Get parameters for the first signal
  auto first = spy.at(0);
  // Check that first parameter contains expected string
  EXPECT_EQ(first.at(0).toString(), "Start file:test/my-file.txt");

  // Obtain parameters for the second generated signal
  auto second = spy.at(1);
  // Test that parameter has expected content
  EXPECT_EQ(second.at(0).toString(), "  test line");
}
