#include "impl/DialogsImpl.h"

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>

using namespace gui;

std::optional<std::string> DialogsImpl::getSearchFolder(
    QMainWindow* parent) const {
  auto dirToSearch = QFileDialog::getExistingDirectory(
      parent, "Select start folder to search");
  if (dirToSearch.isEmpty()) {
    return {};
  }

  return dirToSearch.toStdString();
}
void DialogsImpl::warning(QMainWindow* parent, const QString& title,
                          const QString& message) const {
  QMessageBox::warning(parent, title, message);
}

void DialogsImpl::information(QMainWindow* parent, const QString& title,
                              const QString& message) const {
  QMessageBox::information(parent, title, message);
}
