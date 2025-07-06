#pragma once

#include "files-search/MatchesCollector.h"
#include "impl/interfaces/DirectoryWalker.h"

namespace files_search {
class DirectoryWalkerImpl : public DirectoryWalker {
 public:
  [[nodiscard]] std::expected<void, std::string> walk(
      const std::filesystem::path& path, const MatchSeeker& seeker,
      MatchesCollector& collector) const override;
};
}  // namespace files_search