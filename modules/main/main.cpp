#include <QApplication>

#include "files-search/Factory.h"
#include "files-search/Searcher.h"
#include "gui/Factory.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  auto mainWindow = gui::makeMainWindow(files_search::makeSearcher());
  mainWindow->show();
  return app.exec();
}