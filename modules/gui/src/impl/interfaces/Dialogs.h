#pragma once

#include <QString>
#include <optional>
#include <string>

class QMainWindow;

namespace gui {
class Dialogs {
 public:
  virtual ~Dialogs() = default;

 public:
  [[nodiscard]] virtual std::optional<std::string> getSearchFolder(
      QMainWindow *parent) const = 0;
  virtual void warning(QMainWindow *parent, const QString &title,
                       const QString &message) const = 0;
  virtual void information(QMainWindow *parent, const QString &title,
                           const QString &message) const = 0;
};
}  // namespace gui