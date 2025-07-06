#include <gmock/gmock.h>

#include "impl/interfaces/DirectoryWalker.h"

class DirectoryWalkerMock : public files_search::DirectoryWalker {
 public:
  MOCK_METHOD((std::expected<void, std::string>), walk,
              (const std::filesystem::path& path,
               const files_search::MatchSeeker& seeker,
               files_search::MatchesCollector& collector),
              (const, override));
};