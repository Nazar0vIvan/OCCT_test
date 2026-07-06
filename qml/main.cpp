#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

#include "3d/occt/occcontroller.h"

#include <Font_FontMgr.hxx>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  const Handle(Font_FontMgr) fontMgr = Font_FontMgr::GetInstance();
  fontMgr->InitFontDataBase();

  // Must outlive QQmlApplicationEngine.
  OccController occController;

  qmlRegisterSingletonInstance("OCCT_test.Backend", 1, 0, "OccController", &occController);

  QQmlApplicationEngine engine;

  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("OCCT_test", "Main");

  return app.exec();
}
