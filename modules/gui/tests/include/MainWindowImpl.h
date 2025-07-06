#pragma once

#include "impl/MainWindow.h"

// Required to enable protected methods in `gui::MainWindow`
// for the test cases
class MainWindowImpl : public gui::MainWindow {
 public:
  using MainWindow::MainWindow;

 public:
  using MainWindow::regex;
  using MainWindow::search;
  using MainWindow::searchOutput;
};