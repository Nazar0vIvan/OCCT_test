#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

#include "occcontroller.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Must outlive QQmlApplicationEngine.
  OccController occController;

  qmlRegisterSingletonInstance(
    "OCCT_test.Backend",
    1,
    0,
    "OccController",
    &occController
  );

  QQmlApplicationEngine engine;

  QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreationFailed,
      &app,
      []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection
      );

  engine.loadFromModule("OCCT_test", "Main");

  return app.exec();
}
