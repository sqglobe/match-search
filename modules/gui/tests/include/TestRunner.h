#pragma once

#include <gmock/gmock.h>

#include <QApplication>
#include <QObject>
#include <QTest>
#include <concepts>

// Runs provided tests with Qt
template <std::derived_from<QObject> T>
int testRun(int argc, char *argv[]) {
  // Setup GMock to throw an exception in the case of failed expectations
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  // Init GMock
  ::testing::InitGoogleMock(&argc, argv);
  // Create Qt-application that will contain event loop
  // and handle events
  QApplication app(argc, argv);
  // Constract tested class
  T test;
  // Run tests
  return QTest::qExec(&test, argc, argv);
}