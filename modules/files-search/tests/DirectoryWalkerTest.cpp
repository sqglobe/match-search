#include <gtest/gtest.h>
#include <match_search/testing/expected_matcher.h>
#include <match_search/testing/tmp_folder.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "files-search/MatchesCollectorMock.h"
#include "impl/DirectoryWalkerImpl.h"
#include "mocks/MatchSeekerMock.h"

using match_search::testing::HasError;
using match_search::testing::IsExpected;
using ::testing::_;
using ::testing::StrictMock;
using namespace files_search;
namespace fs = std::filesystem;

class DirectoryWalkerTest : public testing::Test {
 protected:
  // Set up testing environment
  // This method is used instead of a constructor, while in the constructor
  // isn't possible to add `ASSERT_*` macro to check if file and folders
  // are properly constructed
  void SetUp() override {
    // Store generated name in the member for further usage
    RootTestFolder =
        match_search::testing::temporaryFolder("DirectoryWalkerTest");
    ASSERT_TRUE(fs::exists(RootTestFolder));

    // Construct a folder that remaining empty
    ASSERT_TRUE(fs::create_directory(RootTestFolder / "emptyFolder"));
    // Create folder with files
    ASSERT_TRUE(fs::create_directory(RootTestFolder / "folderWithFiles"));

    auto textFilePath = RootTestFolder / "folderWithFiles" / "text.txt";
    // Create a text file that will be then opened successfully
    std::ofstream textFile(textFilePath.native());
    // Check that file is opened
    ASSERT_TRUE(textFile.is_open());
    // Create a hardlink to the created file in the directory above
    fs::create_hard_link(RootTestFolder / "folderWithFiles" / "text.txt",
                         RootTestFolder / "hardlink.txt");
    // Create a symlink to the file in the same folder
    fs::create_symlink(RootTestFolder / "folderWithFiles" / "text.txt",
                       RootTestFolder / "folderWithFiles" / "symlink.txt");

    {
      auto noPermFilePath = RootTestFolder / "folderWithFiles" / "no-perms.txt";
      // Create file that won't have any permissions to open it
      // to check if this case is properly handled
      std::ofstream noPermFile(noPermFilePath.native());
      ASSERT_TRUE(noPermFile.is_open());
    }
    // Change the permission and prohibit to open this file for everyone
    fs::permissions(RootTestFolder / "folderWithFiles" / "no-perms.txt",
                    fs::perms::none);
  }

  // Clean up step for each test-case
  void TearDown() override {
    auto cnt = fs::remove_all(RootTestFolder);
    ASSERT_GT(cnt, 0);
  }

 protected:
  fs::path RootTestFolder;
  StrictMock<MatchSeekerMock> m_matchSeekerMock;
  StrictMock<MatchesCollectorMock> m_matchCollectorMock;
  DirectoryWalkerImpl m_walker;
};

TEST_F(DirectoryWalkerTest, nonExistingFolder) {
  auto res = m_walker.walk(fs::temp_directory_path() / "my-test" / "folder",
                           m_matchSeekerMock, m_matchCollectorMock);

  EXPECT_THAT(res, HasError("No such file or directory"));
}

TEST_F(DirectoryWalkerTest, emptyFolder) {
  auto res = m_walker.walk(RootTestFolder / "emptyFolder", m_matchSeekerMock,
                           m_matchCollectorMock);
  EXPECT_THAT(res, IsExpected());
}

TEST_F(DirectoryWalkerTest, withoutSubfolders) {
  EXPECT_CALL(m_matchSeekerMock, seek(_, _)).Times(2);

  EXPECT_CALL(
      m_matchCollectorMock,
      failed((RootTestFolder / "folderWithFiles" / "no-perms.txt").c_str()));

  EXPECT_CALL(
      m_matchCollectorMock,
      startFile((RootTestFolder / "folderWithFiles" / "text.txt").c_str()));
  EXPECT_CALL(
      m_matchCollectorMock,
      startFile((RootTestFolder / "folderWithFiles" / "symlink.txt").c_str()));

  auto res = m_walker.walk(RootTestFolder / "folderWithFiles",
                           m_matchSeekerMock, m_matchCollectorMock);
  EXPECT_THAT(res, IsExpected());
}

TEST_F(DirectoryWalkerTest, withSubfolders) {
  EXPECT_CALL(m_matchSeekerMock, seek(_, _)).Times(3);

  EXPECT_CALL(m_matchCollectorMock,
              startFile((RootTestFolder / "hardlink.txt").c_str()));

  EXPECT_CALL(
      m_matchCollectorMock,
      failed((RootTestFolder / "folderWithFiles" / "no-perms.txt").c_str()));

  EXPECT_CALL(
      m_matchCollectorMock,
      startFile((RootTestFolder / "folderWithFiles" / "text.txt").c_str()));
  EXPECT_CALL(
      m_matchCollectorMock,
      startFile((RootTestFolder / "folderWithFiles" / "symlink.txt").c_str()));

  auto res =
      m_walker.walk(RootTestFolder, m_matchSeekerMock, m_matchCollectorMock);
  EXPECT_THAT(res, IsExpected());
}