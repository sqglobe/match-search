#include "impl/PlainTextMatchCollector.h"

#include <QPlainTextEdit>

using namespace gui;

void PlainTextMatchCollector::failed(std::string_view path) {
  QString line("Filed to parse: ");
  line.append(path.data());
  emit append(line);
}

void PlainTextMatchCollector::startFile(std::string_view path) {
  QString line("Start file:");
  line.append(path.data());
  emit append(line);
}

void PlainTextMatchCollector::matchedLine(std::string_view line) {
  QString text("  ");
  text.append(line.data());
  emit append(text);
}
