#pragma once

#include "occinputcontroller.h"
#include "occscene.h"
#include "occviewer.h"

#include "3d/robot/model/robotmodel.h"
#include "3d/robot/visualization/robotoccsceneadapter.h"

#include <optional>

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
  void initializeRobotScene(const QString& cadDirectory);
  void updateSceneView();
  void applyInputResult(const OccInputResult& result);

private:
  OccViewer m_viewer;
  OccScene m_scene;
  RobotOccSceneAdapter m_robotOccSceneAdapter;
  OccInputController m_input;

  std::optional<RobotModel> m_robotModel;
};