#include "occviewwindow.h"
#include "occviewport.h"

#include <QExposeEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>

#include <Aspect_Handle.hxx>

#include <cmath>

OccViewWindow::OccViewWindow(QWindow* parent) : QWindow(parent)
{
  setSurfaceType(QSurface::OpenGLSurface);
}

OccViewWindow::~OccViewWindow()
{
  m_viewport.reset();
}

void OccViewWindow::exposeEvent(QExposeEvent* event)
{
  Q_UNUSED(event);

  if (!isExposed()) return;

  initializeViewportIfNeeded();

  if (m_viewport) {
    m_viewport->redraw();
  }
}

void OccViewWindow::resizeEvent(QResizeEvent* event)
{
  Q_UNUSED(event);

  if (m_viewport) {
    m_viewport->resize();
  }
}

void OccViewWindow::mousePressEvent(QMouseEvent* event)
{
  if (!m_viewport) return;

  m_viewport->mousePress(toNativePos(event->position()), event->button());

  event->accept();
}

void OccViewWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (!m_viewport) return;

  m_viewport->mouseMove(toNativePos(event->position()));

  event->accept();
}

void OccViewWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (!m_viewport) return;

  m_viewport->mouseRelease(event->button());

  event->accept();
}

void OccViewWindow::wheelEvent(QWheelEvent* event)
{
  if (!m_viewport) return;

  m_viewport->wheel(toNativePos(event->position()), event->angleDelta().y());

  event->accept();
}

void OccViewWindow::initializeViewportIfNeeded()
{
  if (m_viewport) return;

  m_viewport = std::make_unique<OccViewport>(
      reinterpret_cast<Aspect_Handle>(winId()),
      QStringLiteral(OCCT_TEST_CAD_DIR)
  );
}

QPoint OccViewWindow::toNativePos(const QPointF& pos) const
{
  const qreal dpr = devicePixelRatio();

  return {
    static_cast<int>(std::lround(pos.x() * dpr)),
    static_cast<int>(std::lround(pos.y() * dpr))
  };
}
