#include "files-search/Factory.h"

#include "impl/DirectoryWalkerImpl.h"
#include "impl/SearcherImpl.h"

namespace files_search {
std::unique_ptr<Searcher> makeSearcher() {
  return std::make_unique<SearcherImpl>(
      std::make_unique<DirectoryWalkerImpl>());
}
}  // namespace files_search
