#include <gmock/gmock.h>
#include <match_search/testing/tmp_folder.h>

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
using ::testing::AllOf;
using ::testing::ContainsRegex;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;
namespace fs = std::filesystem;

using namespace gui;

// Enable printing QString with GMock
void PrintTo(const QString &s, ::std::ostream *os) {
  *os << "'" << s.toStdString() << "'";
}

class MainWindowIntegrationTest : public testing::Test {
 protected:
  // Setup environment before each test-case invocation
  void SetUp() override;
  // Cleanup step after each test-case
  void TearDown() override;

 protected:
  std::unique_ptr<MainWindowImpl> m_mainWindow;
  StrictMock<DialogsMock> *m_dialogs = nullptr;
  fs::path RootTestFolder;
};

void MainWindowIntegrationTest::SetUp() {
  // Construct a mock
  auto dialogs = std::make_unique<StrictMock<DialogsMock>>();
  m_dialogs = dialogs.get();

  // Create a main window
  m_mainWindow = std::make_unique<MainWindowImpl>(files_search::makeSearcher(),
                                                  std::move(dialogs));
  RootTestFolder =
      match_search::testing::temporaryFolder("MainWindowIntegrationTest");
  ASSERT_TRUE(fs::exists(RootTestFolder));

  // The empty folder to check that passing empty folder results no founds
  ASSERT_TRUE(fs::create_directory(RootTestFolder / "emptyFolder"));

  // Folder that contains real files
  ASSERT_TRUE(fs::create_directory(RootTestFolder / "folderWithFiles"));

  {
    auto textFilePath = RootTestFolder / "folderWithFiles" / "text.txt";
    // Create text file with content to check if search is working
    std::ofstream textFile(textFilePath.native());
    ASSERT_TRUE(textFile.is_open());
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
    ASSERT_TRUE(noPermFile.is_open());
  }
  fs::permissions(RootTestFolder / "folderWithFiles" / "no-perms.txt",
                  fs::perms::none);
}

void MainWindowIntegrationTest::TearDown() {
  // Destroy main window
  m_mainWindow.reset();
  // Cleanup mock
  m_dialogs = nullptr;
  // Delete temporary folder
  auto cnt = fs::remove_all(RootTestFolder);
  // Check that folder is removed
  ASSERT_GE(cnt, 0);
}

TEST_F(MainWindowIntegrationTest, selectedNonExistingFolder) {
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
  EXPECT_TRUE(finish.wait(1000));
}

TEST_F(MainWindowIntegrationTest, invalidRegex) {
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
  EXPECT_TRUE(finish.wait(1000));
}

TEST_F(MainWindowIntegrationTest, searchCanceled) {
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
  EXPECT_EQ(finish.count(), 0);
}

TEST_F(MainWindowIntegrationTest, emptyFolderSelected) {
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
  EXPECT_TRUE(finish.wait(1000));
  EXPECT_EQ(m_mainWindow->searchOutput()->toPlainText(), "");
}

TEST_F(MainWindowIntegrationTest, directoryWithoutSubfoldersAreSelected) {
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
  EXPECT_TRUE(finish.wait(1000));

  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();
  EXPECT_THAT(
      resultingText.toStdString(),
      AllOf(ContainsRegex("Filed to parse: .+/folderWithFiles/no-perms.txt"),
            ContainsRegex(
                "Start file:.+/folderWithFiles/text.txt\n  One line test"),
            ContainsRegex(
                "Start file:.+/folderWithFiles/symlink.txt\n  One line test")));

  // Check that total number of lines in the resulted text is "5"
  EXPECT_EQ(resultingText.count('\n'), 4);
}

TEST_F(MainWindowIntegrationTest, directoryWithSubfoldersAreSelected) {
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
  EXPECT_TRUE(finish.wait(1000));

  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();
  EXPECT_THAT(
      resultingText.toStdString(),
      AllOf(ContainsRegex("Filed to parse: .+/folderWithFiles/no-perms.txt"),
            ContainsRegex(
                "Start file:.+/folderWithFiles/text.txt\n  One line test"),
            ContainsRegex(
                "Start file:.+/folderWithFiles/symlink.txt\n  One line test"),
            ContainsRegex("Start file:.+/hardlink.txt\n  One line test")));

  // Check that total number of lines in the resulted text is "7"
  EXPECT_EQ(resultingText.count('\n'), 6);
}

TEST_F(MainWindowIntegrationTest, regexNotMatch) {
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
  EXPECT_TRUE(finish.wait(1000));
  // Obtain the text from `searchOutput`
  auto resultingText = m_mainWindow->searchOutput()->toPlainText();
  EXPECT_THAT(
      resultingText.toStdString(),
      AllOf(ContainsRegex("Filed to parse: .+/folderWithFiles/no-perms.txt"),
            ContainsRegex("Start file:.+/folderWithFiles/text.txt"),
            ContainsRegex("Start file:.+/folderWithFiles/symlink.txt")));
  // Check that total number of lines in the resulted text is "3"
  EXPECT_EQ(resultingText.count('\n'), 2);
}