#include <gmock/gmock.h>

#include "files-search/MatchesCollector.h"

namespace files_search {
class MatchesCollectorMock : public MatchesCollector {
 public:
  MOCK_METHOD(void, failed, (std::string_view path), (override));
  MOCK_METHOD(void, startFile, (std::string_view path), (override));
  MOCK_METHOD(void, matchedLine, (std::string_view line), (override));
};
}  // namespace files_search