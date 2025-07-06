#pragma once

#include <QObject>

#include "files-search/MatchesCollector.h"

namespace gui {
class PlainTextMatchCollector : public QObject,
                                public files_search::MatchesCollector {
  Q_OBJECT

 public:
  void failed(std::string_view path) override;
  void startFile(std::string_view path) override;
  void matchedLine(std::string_view line) override;
 signals:
  void append(const QString &s);
};
}  // namespace gui