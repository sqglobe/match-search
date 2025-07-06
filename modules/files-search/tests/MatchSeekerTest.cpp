#include <gtest/gtest.h>

#include <sstream>

#include "files-search/MatchesCollectorMock.h"
#include "impl/MatchSeekerImpl.h"

using namespace files_search;
using ::testing::_;

TEST(MatchSeekerTest, EmptyStream) {
  std::stringstream ss;
  MatchesCollectorMock matchesCollector;
  EXPECT_CALL(matchesCollector, matchedLine(_)).Times(0);
  auto seeker = MatchSeekerImpl(std::regex(".+"));
  seeker.seek(ss, matchesCollector);
}

TEST(MatchSeekerTest, FindOneLine) {
  std::stringstream ss;
  ss << "one row\n"
     << "another one\n"
     << "third line\n";
  MatchesCollectorMock matchesCollector;
  EXPECT_CALL(matchesCollector, matchedLine("third line"));

  auto seeker = MatchSeekerImpl(std::regex(".+line"));
  seeker.seek(ss, matchesCollector);
}

TEST(MatchSeekerTest, MatchTwoLines) {
  std::stringstream ss;
  ss << "one row\n"
     << "another row\n"
     << "third line\n";
  MatchesCollectorMock matchesCollector;

  EXPECT_CALL(matchesCollector, matchedLine("third line"));
  EXPECT_CALL(matchesCollector, matchedLine("one row"));

  auto seeker = MatchSeekerImpl(std::regex("one|third"));
  seeker.seek(ss, matchesCollector);
}