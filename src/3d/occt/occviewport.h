#pragma once

#include "occinputcontroller.h"
#include "occscene.h"
#include "occviewer.h"

#include "3d/robot/kinematics/kr10kinematics.h"
#include "3d/robot/model/kr10model.h"
#include "3d/robot/visualization/robotoccsceneadapter.h"

#include <optional>

#include <QPoint>
#include <QString>
#include <Qt>
#include <QTimer>

#include <Aspect_Handle.hxx>

class OccViewport final
{
public:
  OccViewport(Aspect_Handle handle, const QString& cadDirectory);
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

  void solveIK(const V6d& pose);
  void solveFK(const V6d& q);

private:
  void initializeStaticScene();
  void initializeRobotScene(const QString& cadDirectory);
  void updateSceneView();

  void applyInputResult(const OccInputResult& input);

  void setupRenderTimer();
  void requestRender(bool needsUpdate, bool needsRedraw);
  void flushRenderRequest();

private:
  OccViewer m_viewer;
  OccScene m_scene;
  RobotOccSceneAdapter m_robotAdapter;
  OccInputController m_input;
  std::optional<Kr10Model> m_kr10;
  std::optional<Kr10Kinematics> m_kin;

  QTimer m_timer;
  bool m_update = false;
  bool m_redraw = false;
};