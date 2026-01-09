#include <gtest/gtest.h>
#include <match_search/testing/expected_matcher.h>

TEST(ExpectedMatcherTest, IsExpected) {
  std::expected<int, std::string> expected = 100;
  EXPECT_THAT(expected, match_search::testing::IsExpected());
  EXPECT_THAT(expected, match_search::testing::IsExpected(::testing::Ge(90)));

  EXPECT_THAT(expected, match_search::testing::IsExpected(::testing::Eq(100)));

  std::expected<int, std::string> unexpected = std::unexpected("an error");
  EXPECT_THAT(unexpected, ::testing::Not(match_search::testing::IsExpected()));
}

TEST(ExpectedMatcherTest, HasError) {
  std::expected<int, std::string> unexpected = std::unexpected("an error");
  EXPECT_THAT(unexpected, match_search::testing::HasError());
  EXPECT_THAT(unexpected, match_search::testing::HasError(
                              ::testing::StrCaseEq("an error")));
  EXPECT_THAT(unexpected,
              match_search::testing::HasError(::testing::StrNe("An error")));

  std::expected<int, std::string> expected = 100;
  EXPECT_THAT(expected, ::testing::Not(match_search::testing::HasError()));
}