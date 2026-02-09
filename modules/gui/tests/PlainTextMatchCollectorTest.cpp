#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTest>

#include "impl/PlainTextMatchCollector.h"

using namespace gui;
using ::testing::ElementsAre;

TEST(PlainTextMatchCollectorTest, failure) {
  // Create tested class
  PlainTextMatchCollector collector;
  // Connect spy to the signal to collect if something is emitted
  QSignalSpy spy(&collector, &PlainTextMatchCollector::append);
  // Call the function and expect that signal will be emitted
  collector.failed("test/my-file.txt");
  // Check the generated signal has expected parameters
  EXPECT_THAT(
      spy, ElementsAre(QList<QVariant>({"Filed to parse: test/my-file.txt"})));
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

  // Check the generated two signals has expected parameters
  EXPECT_THAT(spy, ElementsAre(QList<QVariant>({"Start file:test/my-file.txt"}),
                               QList<QVariant>({"  test line"})));
}
