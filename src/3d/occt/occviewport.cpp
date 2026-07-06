#include "occviewport.h"

#include "3d/robot/kinematics/forwardkinematics.h"

#include <array>

#include <QDebug>
#include <QDir>

OccViewport::OccViewport(const Aspect_Handle nativeWindowHandle, const QString& cadDirectory)
    : m_viewer(nativeWindowHandle),
      m_scene(m_viewer.context(), m_viewer.view(), cadDirectory),
      m_robotOccSceneAdapter(m_scene),
      m_input(m_viewer.context(), m_viewer.view())
{
  initializeStaticScene();
  initializeRobotScene(cadDirectory);
  updateSceneView();
}

bool OccViewport::isValid() const
{
  return m_viewer.isValid()
      && m_scene.isValid()
      && m_input.isValid();
}

void OccViewport::resize()
{
  if (!m_viewer.isValid()) return;

  m_viewer.resize();

  if (m_scene.isValid()) {
    m_scene.updateCameraDependentObjects();
  }

  m_viewer.updateCurrentViewer();
  m_viewer.redraw();
}

void OccViewport::redraw()
{
  if (!m_viewer.isValid()) return;

  m_viewer.redraw();
}

void OccViewport::mousePress(const QPoint& pos,const Qt::MouseButton button)
{
  if (!isValid()) return;

  applyInputResult(m_input.mousePress(pos, button));
}

void OccViewport::mouseMove(const QPoint& pos)
{
  if (!isValid()) return;

  applyInputResult(m_input.mouseMove(pos));
}

void OccViewport::mouseRelease(const Qt::MouseButton button)
{
  if (!isValid()) return;

  applyInputResult(m_input.mouseRelease(button));
}

void OccViewport::wheel(const QPoint& pos, const int angleDeltaY)
{
  if (!isValid()) return;

  applyInputResult(m_input.wheel(pos, angleDeltaY));
}

void OccViewport::initializeStaticScene()
{
  if (!isValid()) {
    qWarning() << "Cannot initialize OCCT viewport: invalid viewer, scene, or input controller";
    return;
  }

  m_scene.displayInfrastructure();
}

void OccViewport::initializeRobotScene(const QString& cadDirectory)
{
  if (!isValid()) {
    qWarning() << "Cannot initialize robot scene: invalid viewer, scene, or input controller";
    return;
  }

  const QDir cadDir(cadDirectory);

  const QString robotModelFileName = cadDir.filePath(QStringLiteral("KR10.json"));

  m_robotModel = RobotModel::fromFile(robotModelFileName);

  if (!m_robotModel) {
    qWarning() << "Cannot load robot model:" << robotModelFileName;
    return;
  }

  if (!m_robotOccSceneAdapter.load(m_robotModel->links)) {
    qWarning() << "Cannot load robot links into OCCT scene";
    return;
  }

  const std::array<M4d, LinkCount> transforms = ForwardKinematics::linkTransforms(*m_robotModel, m_robotModel->qHome);

  if (!m_robotOccSceneAdapter.applyTransforms(transforms)) {
    qWarning() << "Cannot apply robot HOME transforms";
    return;
  }
}

void OccViewport::updateSceneView()
{
  if (!m_viewer.isValid()) return;

  m_viewer.fitAll();

  if (m_scene.isValid()) {
    m_scene.updateCameraDependentObjects();
  }

  m_viewer.updateCurrentViewer();
  m_viewer.redraw();
}

void OccViewport::applyInputResult(const OccInputResult& result)
{
  if (!result.accepted) return;

  if (result.cameraChanged) {
    m_scene.updateCameraDependentObjects();
  }

  if (result.needsViewerUpdate) {
    m_viewer.updateCurrentViewer();
  }

  if (result.needsRedraw) {
    m_viewer.redraw();
  }
}