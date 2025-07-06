#include <memory>
#include <regex>

#include "files-search/MatchesCollector.h"
#include "impl/interfaces/MatchSeeker.h"

namespace files_search {
class MatchSeekerImpl : public MatchSeeker {
 public:
  explicit MatchSeekerImpl(std::regex regex);

 public:
  void seek(std::istream& is, MatchesCollector& collector) const override;

 private:
  std::regex m_regex;
};
}  // namespace files_search