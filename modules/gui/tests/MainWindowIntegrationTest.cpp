#include <gmock/gmock.h>

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
#include "TestRunner.h"
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
  void init();
  // Cleanup step after each test-case
  void cleanup();
 private slots:
  void selectedNonExistingFolderTest();
  void invalidRegexTest();
  void searchCanceledTest();
  void emptyFolderSelectedTest();
  void directoryWithoutSubfoldersAreSelectedTest();
  void directoryWithSubfoldersAreSelectedTest();
  void regexNotMatchTest();

 private:
  std::unique_ptr<MainWindowImpl> m_mainWindow;
  StrictMock<DialogsMock> *m_dialogs = nullptr;
  fs::path RootTestFolder;
};

void MainWindowIntegrationTest::init() {
  // Construct a mock
  auto dialogs = std::make_unique<StrictMock<DialogsMock>>();
  m_dialogs = dialogs.get();

  // Create a main window
  m_mainWindow = std::make_unique<MainWindowImpl>(files_search::makeSearcher(),
                                                  std::move(dialogs));

  // Generate rundom name for the test to store files and folder
  auto folderName = "MainWindowIntegrationTest_" + std::to_string(std::rand());
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

void MainWindowIntegrationTest::cleanup() {
  // Destroy main window
  m_mainWindow.reset();
  // Cleanup mock
  m_dialogs = nullptr;
  // Delete temporary folder
  auto cnt = fs::remove_all(RootTestFolder);
  // Check that folder is removed
  QCOMPARE_GT(cnt, 0);
}

void MainWindowIntegrationTest::selectedNonExistingFolderTest() {
  // Construct path to non-existing folder
  auto selectedFolder = (RootTestFolder / "non-existing-folder").native();
  // Emulate that user selected that folder
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{selectedFolder}));
  // Setup expectation that user will get a proper warning
  EXPECT_CALL(*m_dialogs, warning(m_mainWindow.get(), QString("Search failed"),
                                  QString("No such file or directory")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));
}

void MainWindowIntegrationTest::invalidRegexTest() {
  // Emulate that user selected a proper folder for search
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{RootTestFolder.native()}));
  // Setup an expectation that warning will be shown
  EXPECT_CALL(*m_dialogs,
              warning(m_mainWindow.get(), QString("Search failed"), _));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering incorrect regular expression
  QTest::keyClicks(m_mainWindow->regex(), "test((*a");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));
}

void MainWindowIntegrationTest::searchCanceledTest() {
  // Return an empty `std::optional` to emulate that the user
  // canceled search
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{}));
  // Setup expectation, that message about search cancellation is shown
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Canceled"),
                                      QString("Search was canceled")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Check that no signal about process finishing is emitted, while search
  // wasn't started at all
  QCOMPARE(finish.count(), 0);
}

void MainWindowIntegrationTest::emptyFolderSelectedTest() {
  auto selectedFolder = (RootTestFolder / "emptyFolder").native();
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{selectedFolder}));
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Finished"),
                                      QString("Search finished successfully")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression
  QTest::keyClicks(m_mainWindow->regex(), "test*a");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));
  QCOMPARE(m_mainWindow->searchOutput()->toPlainText(), "");
}

void MainWindowIntegrationTest::directoryWithoutSubfoldersAreSelectedTest() {
  // Build the path to the folder, that contains only files without subfolders
  auto selectedFolder = (RootTestFolder / "folderWithFiles").native();
  // Emulate that user selected previously constructed path
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{selectedFolder}));
  // Setup an expectation that the user will obtain a message about success
  // search finish
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Finished"),
                                      QString("Search finished successfully")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression, that matches to a single line
  // in the text file
  QTest::keyClicks(m_mainWindow->regex(), "On. line");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));

  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();
  {
    // Construct message about failure to open files without any permission
    auto failedToFind =
        QString("Filed to parse: %1/no-perms.txt").arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(failedToFind));
  }

  {
    // Construct message about starting with text file and finding single match
    auto fileFound = QString("Start file:%1/text.txt\n  One line test")
                         .arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(fileFound));
  }

  {
    // Construct message about starting with symlink and finding single match
    auto symlinkFound = QString("Start file:%1/symlink.txt\n  One line test")
                            .arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(symlinkFound));
  }

  // Check that total number of lines in the resulted text is "5"
  QCOMPARE(resultingText.count('\n'), 4);
}

void MainWindowIntegrationTest::directoryWithSubfoldersAreSelectedTest() {
  // Emulate that user selected root-folder with files and subfolders
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{RootTestFolder}));
  // Setup expectation, that the user will obtain a message about successfully
  // finished search
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Finished"),
                                      QString("Search finished successfully")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression, that matches to a single line
  // in the text file
  QTest::keyClicks(m_mainWindow->regex(), "On. line");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));

  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();

  {
    // Construct a message about failure to open files without any permission
    auto failedToFind =
        QString("Filed to parse: %1/folderWithFiles/no-perms.txt")
            .arg(RootTestFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(failedToFind));
  }

  {
    // Construct a message about starting text file and finding single match in
    // it
    auto fileFound =
        QString("Start file:%1/folderWithFiles/text.txt\n  One line test")
            .arg(RootTestFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(fileFound));
  }

  {
    // Construct a message about starting symlink and finding single match in
    // it
    auto symlinkFound =
        QString("Start file:%1/folderWithFiles/symlink.txt\n  One line test")
            .arg(RootTestFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(symlinkFound));
  }

  {
    // Construct a message about starting hardlink and finding single match in
    // it
    auto hardlink = QString("Start file:%1/hardlink.txt\n  One line test")
                        .arg(RootTestFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(hardlink));
  }
  // Check that total number of lines in the resulted text is "7"
  QCOMPARE(resultingText.count('\n'), 6);
}

void MainWindowIntegrationTest::regexNotMatchTest() {
  auto selectedFolder = (RootTestFolder / "folderWithFiles").native();
  EXPECT_CALL(*m_dialogs, getSearchFolder(m_mainWindow.get()))
      .WillOnce(Return(std::optional<std::string>{selectedFolder}));
  EXPECT_CALL(*m_dialogs, information(m_mainWindow.get(), QString("Finished"),
                                      QString("Search finished successfully")));
  // Create `spy` to catch finish signal
  QSignalSpy finish(m_mainWindow.get(), &MainWindowImpl::finished);
  // Emulate entering proper regular expression
  QTest::keyClicks(m_mainWindow->regex(), "One and two line");
  // Emulate clicking on the "Search" button
  QTest::mouseClick(m_mainWindow->search(), Qt::MouseButton::LeftButton);
  // Wait until the search process is finished and test that timeout isn't
  // exhausted
  QVERIFY(finish.wait(1000));
  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();
  {
    // Construct a message about failure to open files without any permission
    auto failedToFind =
        QString("Filed to parse: %1/no-perms.txt").arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(failedToFind));
  }

  {
    // Construct a message about starting searching matches in the text file
    auto fileFound =
        QString("Start file:%1/text.txt").arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(fileFound));
  }

  {
    // Construct a message about starting searching matches in the symlink
    auto symlinkFound =
        QString("Start file:%1/symlink.txt").arg(selectedFolder.c_str());
    // Check that resulted text contains that message
    QVERIFY(resultingText.contains(symlinkFound));
  }
  // Check that total number of lines in the resulted text is "3"
  QCOMPARE(resultingText.count('\n'), 2);
}

int main(int argc, char *argv[]) {
  return testRun<MainWindowIntegrationTest>(argc, argv);
}

#include "MainWindowIntegrationTest.moc"