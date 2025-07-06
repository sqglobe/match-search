#include <gmock/gmock.h>

#include "files-search/Searcher.h"

namespace files_search {
class SearcherMock : public Searcher {
 public:
  MOCK_METHOD((std::expected<void, std::string>), search,
              (std::string_view path, std::string_view regex,
               files_search::MatchesCollector &collector),
              (const, override));
};
}  // namespace files_search