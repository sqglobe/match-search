#pragma once

#include <QMainWindow>
#include <QThread>
#include <memory>

#include "files-search/Searcher.h"
#include "impl/SearchWorker.h"
#include "impl/interfaces/Dialogs.h"

class QPushButton;
class QLineEdit;
class QPlainTextEdit;

namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(std::unique_ptr<files_search::Searcher> searcher,
             std::unique_ptr<Dialogs> dialogs);

 private slots:
  void searchClicked();
  void searchFinished(const std::expected<void, std::string> &res);

 signals:
  void startSearch(const std::string &path, const std::string &regex);
  void finished();

 protected:
  QPushButton *search() const;
  QLineEdit *regex() const;
  QPlainTextEdit *searchOutput() const;

 private:
  std::unique_ptr<Ui::MainWindow, void (*)(Ui::MainWindow *)> m_ui;
  std::unique_ptr<Dialogs> m_dialogs;
  std::unique_ptr<SearchWorker> m_worker;
  std::unique_ptr<QThread, void (*)(QThread *)> m_thread;
};
}  // namespace gui