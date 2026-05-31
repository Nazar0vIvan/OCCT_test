#pragma once

#include "occviewer.h"
#include "occscene.h"
#include "occinputcontroller.h"

#include <QPoint>
#include <QString>
#include <Qt>

#include <Aspect_Handle.hxx>

class OccViewport final
{
public:
  OccViewport(Aspect_Handle nativeWindowHandle, const QString& cadDirectory);

  ~OccViewport() = default;

  OccViewport(const OccViewport&) = delete;
  OccViewport& operator=(const OccViewport&) = delete;

  OccViewport(OccViewport&&) noexcept = delete;
  OccViewport& operator=(OccViewport&&) noexcept = delete;

  [[nodiscard]] bool isValid() const;

  void resize();
  void redraw();

  void mousePress(const QPoint& pos, Qt::MouseButton button);

  void mouseMove(const QPoint& pos);

  void mouseRelease(Qt::MouseButton button);

  void wheel(const QPoint& pos, int angleDeltaY);

private:
  void initializeStaticScene();
  void applyInputResult(const OccInputResult& result);

private:
  // Declaration order is important.
  //
  // OccViewer must be constructed first because OccScene and OccInputController
  // receive handles from it.
  OccViewer m_viewer;
  OccScene m_scene;
  OccInputController m_input;
};
