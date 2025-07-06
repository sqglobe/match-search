#pragma once

#include "impl/interfaces/Dialogs.h"

namespace gui {
class DialogsImpl : public Dialogs {
 public:
  std::optional<std::string> getSearchFolder(
      QMainWindow *parent) const override;
  void warning(QMainWindow *parent, const QString &title,
               const QString &message) const override;
  void information(QMainWindow *parent, const QString &title,
                   const QString &message) const override;
};
}  // namespace gui