#pragma once

#include <QObject>
#include <memory>
#include <string>

#include "files-search/MatchesCollector.h"
#include "files-search/Searcher.h"

namespace gui {
class SearchWorker : public QObject {
  Q_OBJECT
 public:
  SearchWorker(std::unique_ptr<files_search::MatchesCollector> collector,
               std::unique_ptr<files_search::Searcher> searcher);
 public slots:
  void startSearch(const std::string &path, const std::string &regex);
 signals:
  void searchFinished(const std::expected<void, std::string> &res);
  void searchStarted();

 private:
  std::unique_ptr<files_search::MatchesCollector> m_collector;
  std::unique_ptr<files_search::Searcher> m_searcher;
};
}  // namespace gui