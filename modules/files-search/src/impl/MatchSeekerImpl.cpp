#include "impl/MatchSeekerImpl.h"

using namespace files_search;

MatchSeekerImpl::MatchSeekerImpl(std::regex regex)
    : m_regex(std::move(regex)) {}

void MatchSeekerImpl::seek(std::istream& is,
                           MatchesCollector& collector) const {
  std::string line;
  line.reserve(1000);
  while (std::getline(is, line)) {
    if (std::regex_search(line, m_regex)) {
      collector.matchedLine(line);
    }
  }
}
