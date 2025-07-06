#pragma once

#include <memory>

#include "files-search/Searcher.h"
#include "impl/interfaces/DirectoryWalker.h"

namespace files_search {
class SearcherImpl : public Searcher {
 public:
  explicit SearcherImpl(std::unique_ptr<DirectoryWalker> walker);

 public:
  [[nodiscard]] std::expected<void, std::string> search(
      std::string_view path, std::string_view regex,
      MatchesCollector& collector) const override;

 private:
  std::unique_ptr<DirectoryWalker> m_walker;
};
}  // namespace files_search