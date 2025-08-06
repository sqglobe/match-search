#include <gmock/gmock.h>

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <filesystem>
#include <memory>
#include <optional>

#include "MainWindowImpl.h"
#include "TestRunner.h"
#include "files-search/SearcherMock.h"
#include "mocks/DialogsMock.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;

using namespace gui;
using namespace files_search;

void PrintTo(const QString &s, ::std::ostream *os) {
  *os << "'" << s.toStdString() << "'";
}

class MainWindowTest : public QObject {
  Q_OBJECT
 private slots:
  // Create mocks and tested class before each test-case invocation
  void init();
  void cleanup();
 private slots:
  void folderNotSelectedTest();
  void searchFailedTest();
  void searchSucceedTest();

 private:
  std::unique_ptr<MainWindowImpl> m_mainWindow;
  StrictMock<DialogsMock> *m_dialogs = nullptr;
  StrictMock<SearcherMock> *m_searcher = nullptr;
};

void MainWindowTest::init() {
  // Construct mocks
  auto dialogs = std::make_unique<StrictMock<DialogsMock>>();
  m_dialogs = dialogs.get();

  auto searcher = std::make_unique<StrictMock<SearcherMock>>();
  m_searcher = searcher.get();
  // Create tested class
  m_mainWindow =
      std::make_unique<MainWindowImpl>(std::move(searcher), std::move(dialogs));
}

void MainWindowTest::cleanup() {
  // On cleanup delete tested class and all mocks
  m_mainWindow.reset();
  m_dialogs = nullptr;
  m_searcher = nullptr;
}

void MainWindowTest::folderNotSelectedTest() {
  InSequence s;

  // Call to `getSearchFolder` should return
  // empty `std::optional` to emulate that user canceled folder selection
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{}));

  // Set expectation that in this case message about cancellation is shown
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Canceled"),
                                      QString("Search was canceled")));
  // Fill field with a regular expression with a proper one
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
}

void MainWindowTest::searchFailedTest() {
  // Emulate that use selected some non-existing folder
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{"test/my-folder"}));

  // Setup expectation, that mock is called with expected folder and regular
  // expression and return `std::unexpected` to emulate that search failed
  EXPECT_CALL(*m_searcher, search("test/my-folder", "test*a", _))
      .WillOnce(Return(std::unexpected("Failed to open folder")));

  // Setup expectation that dialog about failed search will be shown with
  // error message obtained from the mock
  EXPECT_CALL(*m_dialogs, warning(m_mainWindow.get(), QString("Search failed"),
                                  QString("Failed to open folder")));
  // Create `spy` to catch a signal, when process is finished
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate typing text into  `regex` field
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on "Search" button, that should run the search process
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

  // Check that signal about search finish is generated without the timeout
  QVERIFY(finish.wait(1000));
}

void MainWindowTest::searchSucceedTest() {
  // Emulate selection of a folder via `dialogs` mock
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{"test/my-folder"}));
  // Setup an expectation that search folder and regular expression
  // are supplied to `m_searcher` without changes, success is returnred
  EXPECT_CALL(*m_searcher, search("test/my-folder", "test*a", _))
      .WillOnce(
          [](std::string_view, std::string_view, MatchesCollector &collector) {
            // emulate a scenario when file is opened and some matches
            // are found to check what would printed into `searchOutput`
            collector.startFile("test/my-folder/my-file.txt");
            collector.matchedLine("one line");
            collector.matchedLine("another line");
            return std::expected<void, std::string>{};
          });
  // Setup expectation, that after success search a proper message is displayed
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Finished"),
                                      QString("Search finished successfully")));

  // Create a `spy` to catch the signal when search is finished
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering regular expression by user
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on "Search" button, that should start all the process
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));

  // Test that content `searchOutput` is the same as expected
  QCOMPARE(m_mainWindow->searchOutput()->toPlainText(),
           "Start file:test/my-folder/my-file.txt\n  one line\n  another line");
}

int main(int argc, char *argv[]) { return testRun<MainWindowTest>(argc, argv); }

#include "MainWindowTest.moc"