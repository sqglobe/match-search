#include <gmock/gmock.h>

#include <QApplication>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>

#include "MainWindowImpl.h"
#include "files-search/Factory.h"
#include "mocks/DialogsMock.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;
namespace fs = std::filesystem;

using namespace gui;

// Enable printing QString with GMock
void PrintTo(const QString &s, ::std::ostream *os) {
  *os << "'" << s.toStdString() << "'";
}

class MainWindowIntegrationTest : public QObject {
  Q_OBJECT
 private slots:
  // Setup environment before each test-case invocation
  void init() {
    auto dialogs = std::make_unique<StrictMock<DialogsMock>>();
    m_dialogs = dialogs.get();

    m_mainWindow = std::make_unique<MainWindowImpl>(
        files_search::makeSearcher(), std::move(dialogs));

    // Generate rundom name for the test to store files and folder
    auto folderName =
        "MainWindowIntegrationTest_" + std::to_string(std::rand());
    // Store that folder for the further usage
    RootTestFolder = fs::temp_directory_path() / folderName;
    // If name collides, regenerated it
    if (fs::exists(RootTestFolder)) {
      auto folderName =
          "MainWindowIntegrationTest_" + std::to_string(std::rand());
      RootTestFolder = fs::temp_directory_path() / folderName;
    }

    // Create the root folder
    QVERIFY(fs::create_directory(RootTestFolder));

    // The empty folder to check that passing empty folder results no founds
    QVERIFY(fs::create_directory(RootTestFolder / "emptyFolder"));

    // Folder that contains real files
    QVERIFY(fs::create_directory(RootTestFolder / "folderWithFiles"));

    {
      auto textFilePath = RootTestFolder / "folderWithFiles" / "text.txt";
      // Create text file with content to check if search is working
      std::ofstream textFile(textFilePath.native());
      QVERIFY(textFile.is_open());
      textFile << "One line test\n"
               << "Another line test\n"
               << "Third line check";
    }

    // Create hardlink in the folder above to verify that algorithm properly
    // handle hardlinks
    fs::create_hard_link(RootTestFolder / "folderWithFiles" / "text.txt",
                         RootTestFolder / "hardlink.txt");

    // Create a symlink in the same folder to check if algorithm works properly
    // with symlinks
    fs::create_symlink(RootTestFolder / "folderWithFiles" / "text.txt",
                       RootTestFolder / "folderWithFiles" / "symlink.txt");

    {
      auto noPermFilePath = RootTestFolder / "folderWithFiles" / "no-perms.txt";
      // Create a file that has no permissions read from, so it will cause error
      std::ofstream noPermFile(noPermFilePath.native());
      QVERIFY(noPermFile.is_open());
    }
    fs::permissions(RootTestFolder / "folderWithFiles" / "no-perms.txt",
                    fs::perms::none);
  }

  // Cleanup step after each test-case
  void cleanup() {
    m_mainWindow.reset();
    m_dialogs = nullptr;
    auto cnt = fs::remove_all(RootTestFolder);
    QCOMPARE_GT(cnt, 0);
  }

  void selectedNonExistingFolderTest() {
    auto selectedFolder = (RootTestFolder / "non-existing-folder").native();
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{selectedFolder}));
    EXPECT_CALL(*m_dialogs,
                warning(m_mainWindow.get(), QString("Search failed"),
                        QString("No such file or directory")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));
  }

  void invalidRegexTest() {
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{RootTestFolder.native()}));
    EXPECT_CALL(*m_dialogs,
                warning(m_mainWindow.get(), QString("Search failed"), _));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "test((*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));
  }

  void searchCanceledTest() {
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{}));
    EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Canceled"),
                                        QString("Search was canceled")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QCOMPARE(finish.count(), 0);
  }

  void emptyFolderSelectedTest() {
    auto selectedFolder = (RootTestFolder / "emptyFolder").native();
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{selectedFolder}));
    EXPECT_CALL(*m_dialogs,
                information(m_mainWindow.get(), QString("Finished"),
                            QString("Search finished successfully")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "test*a");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));
    QCOMPARE(m_mainWindow->searchOutput()->toPlainText(), "");
  }

  void directoryWithoutSubfoldersAreSelectedTest() {
    auto selectedFolder = (RootTestFolder / "folderWithFiles").native();
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{selectedFolder}));
    EXPECT_CALL(*m_dialogs,
                information(m_mainWindow.get(), QString("Finished"),
                            QString("Search finished successfully")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "On. line");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));

    auto resultingText = m_mainWindow->searchOutput()->toPlainText();
    {
      auto failedToFind = QString("Filed to parse: %1/no-perms.txt")
                              .arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(failedToFind));
    }

    {
      auto fileFound = QString("Start file:%1/text.txt\n  One line test")
                           .arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(fileFound));
    }

    {
      auto symlinkFound = QString("Start file:%1/symlink.txt\n  One line test")
                              .arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(symlinkFound));
    }

    QCOMPARE(resultingText.count('\n'), 4);
  }

  void directoryWithSubfoldersAreSelectedTest() {
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{RootTestFolder}));
    EXPECT_CALL(*m_dialogs,
                information(m_mainWindow.get(), QString("Finished"),
                            QString("Search finished successfully")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "On. line");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));

    auto resultingText = m_mainWindow->searchOutput()->toPlainText();

    {
      auto failedToFind =
          QString("Filed to parse: %1/folderWithFiles/no-perms.txt")
              .arg(RootTestFolder.c_str());
      QVERIFY(resultingText.contains(failedToFind));
    }

    {
      auto fileFound =
          QString("Start file:%1/folderWithFiles/text.txt\n  One line test")
              .arg(RootTestFolder.c_str());
      QVERIFY(resultingText.contains(fileFound));
    }

    {
      auto symlinkFound =
          QString("Start file:%1/folderWithFiles/symlink.txt\n  One line test")
              .arg(RootTestFolder.c_str());
      QVERIFY(resultingText.contains(symlinkFound));
    }

    {
      auto hardlink = QString("Start file:%1/hardlink.txt\n  One line test")
                          .arg(RootTestFolder.c_str());
      QVERIFY(resultingText.contains(hardlink));
    }

    QCOMPARE(resultingText.count('\n'), 6);
  }

  void regexNotMatchTest() {
    auto selectedFolder = (RootTestFolder / "folderWithFiles").native();
    EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
        .WillOnce(Return(std::optional<std::string>{selectedFolder}));
    EXPECT_CALL(*m_dialogs,
                information(m_mainWindow.get(), QString("Finished"),
                            QString("Search finished successfully")));

    QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);

    QTest::keyClicks(m_mainWindow->regex(), "One and two line");
    QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);

    QVERIFY(finish.wait(1000));

    auto resultingText = m_mainWindow->searchOutput()->toPlainText();
    {
      auto failedToFind = QString("Filed to parse: %1/no-perms.txt")
                              .arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(failedToFind));
    }

    {
      auto fileFound =
          QString("Start file:%1/text.txt").arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(fileFound));
    }

    {
      auto symlinkFound =
          QString("Start file:%1/symlink.txt").arg(selectedFolder.c_str());
      QVERIFY(resultingText.contains(symlinkFound));
    }

    QCOMPARE(resultingText.count('\n'), 2);
  }

 private:
  std::unique_ptr<MainWindowImpl> m_mainWindow;
  StrictMock<DialogsMock> *m_dialogs = nullptr;
  fs::path RootTestFolder;
};

int main(int argc, char *argv[]) {
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);
  QApplication app(argc, argv);
  MainWindowIntegrationTest test;
  return QTest::qExec(&test, argc, argv);
}

#include "MainWindowIntegrationTest.moc"