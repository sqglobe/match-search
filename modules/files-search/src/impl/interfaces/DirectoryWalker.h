#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "files-search/MatchesCollector.h"
#include "impl/interfaces/MatchSeeker.h"

namespace files_search {

class DirectoryWalker {
public:
  virtual ~DirectoryWalker() = default;

public:
  [[nodiscard]] virtual std::expected<void, std::string>
  walk(const std::filesystem::path &path, const MatchSeeker &seeker,
       MatchesCollector &collector) const = 0;
};
} // namespace files_search