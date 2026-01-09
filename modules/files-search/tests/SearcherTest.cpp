#include <gtest/gtest.h>
#include <match_search/testing/expected_matcher.h>

#include "files-search/MatchesCollectorMock.h"
#include "impl/SearcherImpl.h"
#include "mocks/DirectoryWalkerMock.h"

using match_search::testing::HasError;
using match_search::testing::IsExpected;
using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using namespace files_search;

class SearcherTest : public ::testing::Test {
 public:
  // Create all required for test classes and mocks
  SearcherTest() {
    auto walker = std::make_unique<StrictMock<DirectoryWalkerMock>>();
    m_directoryWalker = walker.get();
    m_searcher = std::make_unique<SearcherImpl>(std::move(walker));
  }

 protected:
  StrictMock<DirectoryWalkerMock>* m_directoryWalker;
  std::unique_ptr<SearcherImpl> m_searcher;
  StrictMock<MatchesCollectorMock> m_matchesCollectorMock;
};

TEST_F(SearcherTest, walkerFailed) {
  EXPECT_CALL(*m_directoryWalker,
              walk(std::filesystem::path("some") / "path", _, _))
      .WillOnce(Return(std::unexpected("Failed")));

  auto res =
      m_searcher->search((std::filesystem::path("some") / "path").c_str(),
                         "1232", m_matchesCollectorMock);

  EXPECT_THAT(res, HasError("Failed"));
}

TEST_F(SearcherTest, oneFileFound) {
  EXPECT_CALL(
      m_matchesCollectorMock,
      startFile((std::filesystem::path("some") / "path" / "some.txt").c_str()));
  EXPECT_CALL(m_matchesCollectorMock, matchedLine("third line"));

  std::stringstream ss;
  ss << "one row\n"
     << "another one\n"
     << "third line\n";
  // Check that passed path to `m_searcher->search` is forwarded to the mock
  // and emulate the real behaviour like opening a file stream and invoking
  // `MatchSeeker::seek` with it
  EXPECT_CALL(*m_directoryWalker,
              walk(std::filesystem::path("some") / "path", _, _))
      .WillOnce(
          [this, &ss](
              const std::filesystem::path&, const MatchSeeker& seeker,
              MatchesCollector& collector) -> std::expected<void, std::string> {
            // Simulate the real behaviour
            collector.startFile("some/path/some.txt");
            // Call then `seek` to test if passed class `MatchSeeker` also works
            // properly and will discover only a single line in the stream
            seeker.seek(ss, collector);
            return {};
          });

  auto res =
      m_searcher->search((std::filesystem::path("some") / "path").c_str(),
                         ".+line", m_matchesCollectorMock);

  EXPECT_THAT(res, IsExpected());
}
