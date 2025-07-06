#include <gmock/gmock.h>

#include "impl/interfaces/MatchSeeker.h"

class MatchSeekerMock : public files_search::MatchSeeker {
 public:
  MOCK_METHOD(void, seek,
              (std::istream & is, files_search::MatchesCollector &collector),
              (const, override));
};