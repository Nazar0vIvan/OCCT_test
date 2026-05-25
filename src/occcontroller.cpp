#include "occcontroller.h"

#include <QDebug>

OccController::OccController(QObject* parent) : QObject(parent), m_viewWindow(std::make_unique<OccViewWindow>())
{
}

OccController::~OccController()
{
  if (m_viewWindow) {
    m_viewWindow->hide();
    m_viewWindow.reset();
  }
}

QWindow* OccController::viewWindow() const
{
  return m_viewWindow.get();
}

bool OccController::importStepFile(const QUrl& fileUrl)
{
  const QString filePath = fileUrl.toLocalFile();

  if (filePath.isEmpty()) {
    qWarning() << "Invalid STEP file URL:" << fileUrl;
    return false;
  }

  return m_viewWindow && m_viewWindow->importStepFile(filePath);
}
