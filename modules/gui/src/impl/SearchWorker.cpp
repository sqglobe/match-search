#include "impl/SearchWorker.h"

using namespace gui;

SearchWorker::SearchWorker(
    std::unique_ptr<files_search::MatchesCollector> collector,
    std::unique_ptr<files_search::Searcher> searcher)
    : m_collector(std::move(collector)), m_searcher(std::move(searcher)) {}

void SearchWorker::startSearch(const std::string &path,
                               const std::string &regex) {
  emit searchStarted();
  auto res = m_searcher->search(path, regex, *m_collector);
  emit searchFinished(res);
}