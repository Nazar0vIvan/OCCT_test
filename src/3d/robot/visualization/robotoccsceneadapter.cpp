#include "robotoccsceneadapter.h"

#include <cstddef>
#include <optional>

#include <QDebug>

RobotOccSceneAdapter::RobotOccSceneAdapter(OccScene& scene) : m_scene(scene)
{}

RobotOccSceneAdapter::Status RobotOccSceneAdapter::load(const std::array<LinkModel, LinkCount>& links, const LinkModel& endEffector)
{
  if (!m_scene.isValid()) {
    qWarning() << "Cannot load robot links: OCCT scene is invalid";
    return Status::Failed;
  }

  if (m_loaded) {
    qWarning() << "Robot links are already loaded";
    return Status::Failed;
  }

  std::array<std::optional<PartId>, LinkCount> linkIds{};

  for (std::size_t i = 0; i < LinkCount; ++i) {
    const std::optional<PartId> partId = m_scene.addStepPartWithId(links[i].fileName, links[i].props);

    if (!partId) {
      qWarning() << "Cannot load robot link" << i;
      return Status::Failed;
    }

    linkIds[i] = *partId;
  }

  const std::optional<PartId> effPartId = m_scene.addStepPartWithId(endEffector.fileName, endEffector.props);

  if (!effPartId) {
    qWarning() << "Robot end effector was not loaded:" << endEffector.fileName << "- robot links will be displayed without end effector";
  } else {
    m_effPartId = *effPartId;
  }

  m_linkIds = linkIds;
  m_loaded = true;

  return Status::Done;
}

RobotOccSceneAdapter::Status RobotOccSceneAdapter::applyTransforms(const std::array<M4d, LinkCount>& T0i)
{
  if (!isReady()) return Status::Failed;

  for (std::size_t i = 0; i < LinkCount; ++i) {
    const std::optional<PartId>& partId = m_linkIds[i];

    if (!partId) {
      qWarning() << "Robot link" << i << "has no OCCT scene part id";
      return Status::Failed;
    }

    if (!m_scene.setPartTransform(*partId, T0i[i])) {
      qWarning() << "Cannot apply transform to robot link" << i;
      return Status::Failed;
    }
  }

  if (m_effPartId && !m_scene.setPartTransform(*m_effPartId, T0i[EndEffIdx])) {
    qWarning() << "Cannot apply transform to robot end effector";
    return Status::Failed;
  }

  return Status::Done;
}

bool RobotOccSceneAdapter::isReady() const
{
  if (!m_scene.isValid()) {
    qWarning() << "RobotOccSceneAdapter is not ready: OCCT scene is invalid";
    return false;
  }

  if (!m_loaded) {
    qWarning() << "RobotOccSceneAdapter is not ready: links are not loaded";
    return false;
  }

  return true;
}