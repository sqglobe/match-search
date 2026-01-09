#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <expected>
#include <iostream>

#include "impl/PlainTextMatchCollector.h"
#include "ui/ui_mainwindow.h"

using namespace gui;

namespace {
void cleanupUi(Ui::MainWindow *window) { delete window; }
}  // namespace

MainWindow::MainWindow(std::unique_ptr<files_search::Searcher> searcher,
                       std::unique_ptr<Dialogs> dialogs)
    : m_ui(new Ui::MainWindow, cleanupUi), m_dialogs(std::move(dialogs)) {
  m_ui->setupUi(this);

  connect(m_ui->search, &QAbstractButton::clicked, this,
          &MainWindow::searchClicked);

  auto collector = std::make_unique<PlainTextMatchCollector>();
  collector->moveToThread(&m_thread);
  connect(collector.get(), &PlainTextMatchCollector::append, m_ui->searchOutput,
          &QPlainTextEdit::appendPlainText);

  m_worker =
      std::make_unique<SearchWorker>(std::move(collector), std::move(searcher));
  m_worker->moveToThread(&m_thread);

  connect(m_worker.get(), &SearchWorker::searchFinished, this,
          &MainWindow::searchFinished);
  connect(m_worker.get(), &SearchWorker::searchStarted, m_ui->searchOutput,
          &QPlainTextEdit::clear);

  connect(this, &MainWindow::startSearch, m_worker.get(),
          &SearchWorker::startSearch);

  m_thread.start();
}

void MainWindow::searchFinished(const std::expected<void, std::string> &res) {
  if (res.has_value()) {
    m_dialogs->information(this, "Finished", "Search finished successfully");
  } else {
    m_dialogs->warning(this, "Search failed",
                       QString::fromStdString(res.error()));
  }
  emit finished();
}

void MainWindow::searchClicked() {
  auto dirToSearch = m_dialogs->getSearchFolder(this);

  if (!dirToSearch) {
    m_dialogs->information(this, "Canceled", "Search was canceled");
    return;
  }

  emit startSearch(dirToSearch.value(), m_ui->regex->text().toStdString());
}

QPushButton *MainWindow::search() const { return m_ui->search; }
QLineEdit *MainWindow::regex() const { return m_ui->regex; }
QPlainTextEdit *MainWindow::searchOutput() const { return m_ui->searchOutput; }