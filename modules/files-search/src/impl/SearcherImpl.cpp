#include "impl/SearcherImpl.h"

#include "files-search/MatchesCollector.h"
#include "impl/MatchSeekerImpl.h"

using namespace files_search;

SearcherImpl::SearcherImpl(std::unique_ptr<DirectoryWalker> walker)
    : m_walker(std::move(walker)) {}

std::expected<void, std::string> SearcherImpl::search(
    std::string_view path, std::string_view regex,
    MatchesCollector& collector) const {
  try {
    auto seeker = MatchSeekerImpl(std::regex(regex.cbegin(), regex.cend()));
    return m_walker->walk(path, seeker, collector);
  } catch (const std::exception& ex) {
    return std::unexpected(ex.what());
  }
}
