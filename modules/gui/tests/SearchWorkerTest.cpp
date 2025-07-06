#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTest>

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
  void paramForwardedTest() {
    auto searcher = std::make_unique<StrictMock<SearcherMock>>();
    EXPECT_CALL(*searcher, search("/test/path", "some regex", _))
        .WillOnce(Return(std::expected<void, std::string>{}));
    auto worker =
        SearchWorker(std::make_unique<StrictMock<MatchesCollectorMock>>(),
                     std::move(searcher));

    QSignalSpy searchStarted(&worker, &SearchWorker::searchStarted);
    QSignalSpy searchFinished(&worker, &SearchWorker::searchFinished);

    worker.startSearch("/test/path", "some regex");

    QCOMPARE(searchStarted.count(), 1);
    QCOMPARE(searchFinished.count(), 1);
    auto argument = searchFinished.takeFirst()
                        .at(0)
                        .value<std::expected<void, std::string>>();
    QVERIFY(argument.has_value());
  }
};

int main(int argc, char *argv[]) {
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  QApplication app(argc, argv);
  SearchWorkerTest test;
  return QTest::qExec(&test, argc, argv);
}

#include "SearchWorkerTest.moc"