#include "occcontroller.h"

#include "occviewwindow.h"

OccController::OccController(QObject* parent) : QObject(parent), m_viewWindow(std::make_unique<OccViewWindow>())
{}

OccController::~OccController()
{
  if (m_viewWindow) {
    m_viewWindow->hide();
  }
}

QWindow* OccController::viewWindow() const
{
  return m_viewWindow.get();
}
