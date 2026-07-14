#include "occviewport.h"

#include "3d/utils.h"

#include <QDebug>
#include <QDir>
#include <QObject>
#include <QStringList>

namespace
{

QString fmtV6(const V6d& vals)
{
  QStringList text;

  for (const double val : vals) {
      text << QString::number(val, 'f', 2);
    }

  return QStringLiteral("[%1]").arg(text.join(QStringLiteral(", ")));
}

} // namespace

OccViewport::OccViewport(const Aspect_Handle handle, const QString& cadDirectory)
    : m_viewer(handle),
      m_scene(m_viewer.context(), m_viewer.view(), cadDirectory),
      m_robotAdapter(m_scene),
      m_input(m_viewer.context(), m_viewer.view())
{
  setupRenderTimer();
  initializeStaticScene();
  initializeRobotScene(cadDirectory);
  updateSceneView();
}

bool OccViewport::isValid() const
{
  return m_viewer.isValid() && m_scene.isValid() && m_input.isValid();
}

void OccViewport::resize()
{
  if (!m_viewer.isValid()) return;

  m_viewer.resize();

  if (m_scene.isValid()) {
    m_scene.updateCameraDependentObjects();
  }

  requestRender(true, true);
}

void OccViewport::redraw()
{
  if (!m_viewer.isValid()) return;

  m_viewer.redraw();
}

void OccViewport::mousePress(const QPoint& pos, const Qt::MouseButton button)
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

void OccViewport::solveIK(const V6d& pose)
{
  if (!m_kin) {
    qWarning() << "Cannot solve IK: KR10 kinematics is not initialized";
    return;
  }

  const M4d T06 = makeTransform(euler2rot(pose[3], pose[4], pose[5]), V3d{pose[0], pose[1], pose[2]});

  const std::vector<V6d> qs = m_kin->solveIK(T06);

  qInfo().noquote() << QStringLiteral("IK target %1 -> %2 solution(s)").arg(fmtV6(pose)).arg(qs.size());

  for (std::size_t idx = 0; idx < qs.size(); ++idx) {
    qInfo().noquote() << QStringLiteral("  #%1 q = %2").arg(idx + 1).arg(fmtV6(qs[idx]));
  }
}

void OccViewport::solveFK(const V6d& q)
{
  if (!m_kin) {
    qWarning() << "Cannot solve FK: KR10 kinematics is not initialized";
    return;
  }

  const std::array<M4d, LinkCount> T0i = m_kin->solveFK(q);

  if (m_robotAdapter.applyTransforms(T0i) != RobotOccSceneAdapter::Status::Done) {
    qWarning() << "Cannot apply FK transforms";
    return;
  }

  qInfo().noquote() << QStringLiteral("FK q %1 applied").arg(fmtV6(q));

  requestRender(true, true);
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

  const QString file = cadDir.filePath(QStringLiteral("KR10.json"));
  m_kr10 = Kr10Model::fromJson(file);

  if (!m_kr10) {
    qWarning() << "Cannot load KR10 model:" << file;
    return;
  }

  m_kin.emplace(*m_kr10);


  if (!m_kin->isValid()) {
    qWarning() << "Cannot initialize KR10 kinematics";
    m_kin.reset();
    m_kr10 = std::nullopt;
    return;
  }

  if (m_robotAdapter.load(m_kr10->links, m_kr10->endEffector) != RobotOccSceneAdapter::Status::Done) {
    qWarning() << "Cannot load robot links into OCCT scene";
    m_kin.reset();
    m_kr10 = std::nullopt;
    return;
  }

  const std::array<M4d, LinkCount> T0i = m_kin->solveFK(m_kr10->qHome);

  if (m_robotAdapter.applyTransforms(T0i) != RobotOccSceneAdapter::Status::Done) {
    qWarning() << "Cannot apply robot HOME transforms";
    m_kin.reset();
    m_kr10 = std::nullopt;
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

  requestRender(true, true);
}

void OccViewport::applyInputResult(const OccInputResult& input)
{
  if (!input.accepted) return;

  if (input.cameraScaleChanged) {
    m_scene.updateCameraDependentObjects();
  }

  requestRender(input.needsViewerUpdate, input.needsRedraw);
}

void OccViewport::setupRenderTimer()
{
  m_timer.setSingleShot(true);
  m_timer.setInterval(0);
  m_timer.setTimerType(Qt::PreciseTimer);

  QObject::connect(&m_timer, &QTimer::timeout, [this]() { flushRenderRequest(); });
}

void OccViewport::requestRender(const bool needsUpdate, const bool needsRedraw)
{
  if (!m_viewer.isValid()) return;

  m_update = m_update || needsUpdate;
  m_redraw = m_redraw || needsRedraw;

  if (!m_timer.isActive()) {
    m_timer.start();
  }
}

void OccViewport::flushRenderRequest()
{
  if (!m_viewer.isValid()) {
    m_update = false;
    m_redraw = false;
    return;
  }

  const bool needsUpdate = m_update;
  const bool needsRedraw = m_redraw;

  m_update = false;
  m_redraw = false;

  if (needsUpdate) {
    m_viewer.updateCurrentViewer();
  }

  if (needsRedraw) {
    m_viewer.redraw();
  }
}