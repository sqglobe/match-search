#pragma once

#include <string_view>

namespace files_search {
// Used to notify the interested party about found matches or about errors
class MatchesCollector {
 public:
  virtual ~MatchesCollector() = default;

 public:
  // Method is called if file can't be opened
  virtual void failed(std::string_view path) = 0;
  // Invoked on starting a new file
  virtual void startFile(std::string_view path) = 0;
  // Executed if new match with a regular expression
  virtual void matchedLine(std::string_view line) = 0;
};
}  // namespace files_search