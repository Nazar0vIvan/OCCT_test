#include "robotoccsceneadapter.h"

#include <cstddef>
#include <optional>

#include <QDebug>

RobotOccSceneAdapter::RobotOccSceneAdapter(OccScene& scene) : m_scene(scene)
{}

bool RobotOccSceneAdapter::load(const std::array<LinkModel, LinkCount>& links)
{
  if (!m_scene.isValid()) {
    qWarning() << "Cannot load robot links: OCCT scene is invalid";
    return false;
  }

  if (m_linksLoaded) {
    qWarning() << "Robot links are already loaded";
    return false;
  }

  std::array<std::optional<PartId>, LinkCount> linkPartIds{};

  for (std::size_t i = 0; i < LinkCount; ++i) {
    const std::optional<PartId> partId = m_scene.addStepPartWithId(links[i].fileName, links[i].props);

    if (!partId) {
      qWarning() << "Cannot load robot link" << i;
      return false;
    }

    linkPartIds[i] = *partId;
  }

  m_linkPartIds = linkPartIds;
  m_linksLoaded = true;

  return true;
}

bool RobotOccSceneAdapter::applyTransforms(const std::array<M4d, LinkCount>& transforms)
{
  if (!isReady()) return false;

  for (std::size_t i = 0; i < LinkCount; ++i) {
    const std::optional<PartId>& partId = m_linkPartIds[i];

    if (!partId) {
      qWarning() << "Robot link" << i << "has no OCCT scene part id";
      return false;
    }

    if (!m_scene.setPartTransform(*partId, transforms[i])) {
      qWarning() << "Cannot apply transform to robot link" << i;
      return false;
    }
  }

  m_scene.updateViewer();

  return true;
}

bool RobotOccSceneAdapter::isReady() const
{
  if (!m_scene.isValid()) {
    qWarning() << "RobotOccSceneAdapter is not ready: OCCT scene is invalid";
    return false;
  }

  if (!m_linksLoaded) {
    qWarning() << "RobotOccSceneAdapter is not ready: links are not loaded";
    return false;
  }

  return true;
}