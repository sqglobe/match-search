#include "gui/Factory.h"

#include "impl/DialogsImpl.h"
#include "impl/MainWindow.h"

namespace gui {
std::unique_ptr<QMainWindow> makeMainWindow(
    std::unique_ptr<files_search::Searcher> searcher) {
  return std::make_unique<MainWindow>(std::move(searcher),
                                      std::make_unique<DialogsImpl>());
}
}  // namespace gui
