#pragma once

#include <expected>
#include <string>

namespace files_search {

class MatchesCollector;

// Do a recursive search for a regular expression
// in specified folder
class Searcher {
 public:
  virtual ~Searcher() = default;

 public:
  // This method uses `collector` to notify if any match is found or any file is
  // failed to open.
  // In the case of failure for example if specified `path` is invalid or
  // `regex` is wrong
  [[nodiscard]] virtual std::expected<void, std::string> search(
      std::string_view path, std::string_view regex,
      MatchesCollector &collector) const = 0;
};
}  // namespace files_search