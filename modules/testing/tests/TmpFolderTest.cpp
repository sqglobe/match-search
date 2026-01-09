#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <match_search/testing/tmp_folder.h>

#include <filesystem>

namespace fs = std::filesystem;

TEST(TmpFolderTest, ConstructFolder) {
  auto path = match_search::testing::temporaryFolder("test-folder");
  EXPECT_THAT(path.filename().native(), ::testing::HasSubstr("test-folder_"));
  EXPECT_TRUE(fs::exists(path));
  EXPECT_EQ(path.parent_path(), fs::temp_directory_path());
  EXPECT_TRUE(fs::remove(path));
}

TEST(TmpFolderTest, CreateTwoPaths) {
  auto path1 = match_search::testing::temporaryFolder("test-folder");
  auto path2 = match_search::testing::temporaryFolder("test-folder");
  EXPECT_THAT(path1.filename().native(), ::testing::HasSubstr("test-folder_"));
  EXPECT_THAT(path2.filename().native(), ::testing::HasSubstr("test-folder_"));
  EXPECT_TRUE(fs::exists(path1));
  EXPECT_TRUE(fs::exists(path2));
  EXPECT_EQ(path1.parent_path(), fs::temp_directory_path());
  EXPECT_EQ(path2.parent_path(), fs::temp_directory_path());
  EXPECT_NE(path1, path2);

  EXPECT_TRUE(fs::remove(path1));
  EXPECT_TRUE(fs::remove(path2));
}