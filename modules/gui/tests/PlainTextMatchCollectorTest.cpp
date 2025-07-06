#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTest>

#include "impl/PlainTextMatchCollector.h"

using namespace gui;

class PlainTextMatchCollectorTest : public QObject {
  Q_OBJECT
 private slots:
  void failureTest() {
    PlainTextMatchCollector collector;
    QSignalSpy spy(&collector, &PlainTextMatchCollector::append);

    collector.failed("test/my-file.txt");
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), "Filed to parse: test/my-file.txt");
  }

  void startFileAndMatchTest() {
    PlainTextMatchCollector collector;
    QSignalSpy spy(&collector, &PlainTextMatchCollector::append);
    collector.startFile("test/my-file.txt");
    collector.matchedLine("test line");

    QCOMPARE(spy.count(), 2);

    auto first = spy.at(0);
    QCOMPARE(first.at(0).toString(), "Start file:test/my-file.txt");

    auto second = spy.at(1);
    QCOMPARE(second.at(0).toString(), "  test line");
  }
};

QTEST_MAIN(PlainTextMatchCollectorTest)
#include "PlainTextMatchCollectorTest.moc"