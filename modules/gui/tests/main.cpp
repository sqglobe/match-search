#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[]) {
  QApplication app{argc, argv};

  QTimer::singleShot(0, [&]() {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    auto testResult = RUN_ALL_TESTS();
    app.exit(testResult);
  });

  return app.exec();
}