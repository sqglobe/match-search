#pragma once
#include <QMainWindow>
#include <memory>

namespace files_search {
class Searcher;
}

namespace gui {
// Construct a new main window
// This method helps in hiding the real implementation of
// the class `QMainWindow`
[[nodiscard]] std::unique_ptr<QMainWindow> makeMainWindow(
    std::unique_ptr<files_search::Searcher> searcher);

}  // namespace gui