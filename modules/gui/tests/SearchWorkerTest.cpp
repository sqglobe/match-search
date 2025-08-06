#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTest>

#include "TestRunner.h"
#include "files-search/MatchesCollectorMock.h"
#include "files-search/SearcherMock.h"
#include "impl/SearchWorker.h"

using namespace gui;
using namespace files_search;

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class SearchWorkerTest : public QObject {
  Q_OBJECT
 private slots:
  void paramForwardedTest();
};

void SearchWorkerTest::paramForwardedTest() {
  // Construct mock
  auto searcher = std::make_unique<StrictMock<SearcherMock>>();
  // Setup call expectations for the mock, which returns
  // `std::expected` containing a value
  EXPECT_CALL(*searcher, search("/test/path", "some regex", _))
      .WillOnce(Return(std::expected<void, std::string>{}));
  // Construct tested class
  auto worker =
      SearchWorker(std::make_unique<StrictMock<MatchesCollectorMock>>(),
                   std::move(searcher));
  // Create one `spy` to catch signal about starting searching process
  QSignalSpy searchStarted(&worker, &SearchWorker::searchStarted);
  // Create another `spy` to catch a signal when search is finished
  QSignalSpy searchFinished(&worker, &SearchWorker::searchFinished);

  // Start search process with expected parameters
  // Those parameters should be passed to the mock
  worker.startSearch("/test/path", "some regex");

  // Check that signal about search start is emitted
  QCOMPARE(searchStarted.count(), 1);
  // Test that signal about search finish is emitted
  QCOMPARE(searchFinished.count(), 1);

  // Get the parameters of the first generated signal
  auto argument = searchFinished
                      .takeFirst()
                      // Get the first parameter of the generated signal
                      .at(0)
                      // Cast that parameter to the `std::expected`
                      .value<std::expected<void, std::string>>();
  // Test that `std::expected` contains a value
  QVERIFY(argument.has_value());
}

int main(int argc, char *argv[]) {
  return testRun<SearchWorkerTest>(argc, argv);
}

#include "SearchWorkerTest.moc"