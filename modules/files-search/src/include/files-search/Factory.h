#pragma once

#include <memory>
#include <regex>

namespace files_search {

class Searcher;
// Create a new object
// This method hides the real implementation of the interface
[[nodiscard]] std::unique_ptr<Searcher> makeSearcher();
}  // namespace files_search
