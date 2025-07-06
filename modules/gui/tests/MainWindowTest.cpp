#include <gmock/gmock.h>

#include <QApplication>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <filesystem>
#include <memory>
#include <optional>

#include "MainWindowImpl.h"
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
  void init() {
    auto dialogs = std::make_unique<StrictMock<DialogsMock>>();
    m_dialogs = dialogs.get();

    auto searcher = std::make_unique<StrictMock<SearcherMock>>();
    m_searcher = searcher.get();
    m_mainWindow = std::make_unique<MainWindowImpl>(std::move(searcher),
                                                    std::move(dialogs));
  }

  void cleanup() {
    m_mainWindow.reset();
    m_dialogs = nullptr;
    m_searcher = nullptr;
  }

  void folderNotSelectedTest() {
    InSequence s;

    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{}));
    EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Canceled"),
                                        QString("Search was canceled")));

    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  }

  void searchFailedTest() {
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{"test/my-folder"}));

    EXPECT_CALL(*m_searcher, search("test/my-folder", "test*a", _))
        .WillOnce(Return(std::unexpected("Failed to open folder")));

    EXPECT_CALL(*m_dialogs,
                warning(m_mainWindow.get(), QString("Search failed"),
                        QString("Failed to open folder")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));
  }

  void searchSucceedTest() {
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{"test/my-folder"}));

    EXPECT_CALL(*m_searcher, search("test/my-folder", "test*a", _))
        .WillOnce([](std::string_view, std::string_view,
                     MatchesCollector &collector) {
          collector.startFile("test/my-folder/my-file.txt");
          collector.matchedLine("one line");
          collector.matchedLine("another line");
          return std::expected<void, std::string>{};
        });

    EXPECT_CALL(*m_dialogs,
                information(m_mainWindow.get(), QString("Finished"),
                            QString("Search finished successfully")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
    QVERIFY(finish.wait(1000));

    QCOMPARE(
        m_mainWindow->searchOutput()->toPlainText(),
        "Start file:test/my-folder/my-file.txt\n  one line\n  another line");
  }

 private:
  std::unique_ptr<MainWindowImpl> m_mainWindow;
  StrictMock<DialogsMock> *m_dialogs = nullptr;
  StrictMock<SearcherMock> *m_searcher = nullptr;
};

int main(int argc, char *argv[]) {
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  QApplication app(argc, argv);
  MainWindowTest test;
  return QTest::qExec(&test, argc, argv);
}

#include "MainWindowTest.moc"