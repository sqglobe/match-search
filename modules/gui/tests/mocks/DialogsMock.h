#pragma once

#include <gmock/gmock.h>

#include "impl/interfaces/Dialogs.h"

class DialogsMock : public gui::Dialogs {
 public:
  MOCK_METHOD((std::optional<std::string>), getSearchFolder,
              (QMainWindow * parent), (const, override));
  MOCK_METHOD(void, warning,
              (QMainWindow * parent, const QString &title,
               const QString &message),
              (const, override));
  MOCK_METHOD(void, information,
              (QMainWindow * parent, const QString &title,
               const QString &message),
              (const, override));
};