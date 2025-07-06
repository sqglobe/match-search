#pragma once
#include <istream>

#include "files-search/MatchesCollector.h"

namespace files_search {
class MatchSeeker {
 public:
  virtual ~MatchSeeker() = default;

 public:
  virtual void seek(std::istream &is, MatchesCollector &collector) const = 0;
};
}  // namespace files_search