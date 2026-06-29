#include "robot/visualization/robotsceneadapter.h"

#include <utility>

#include <QDebug>

namespace robot {

RobotSceneAdapter::RobotSceneAdapter(std::shared_ptr<const RobotModel> model, OccScene& scene) : m_model(std::move(model)), m_scene(scene)
{
  if (!m_model) {
    qWarning() << "RobotSceneAdapter created with null RobotModel";
  }
}

void RobotSceneAdapter::setBaseInWorld(const Transform& baseInWorld)
{
  m_baseInWorld = baseInWorld;
}

bool RobotSceneAdapter::loadVisuals(const RobotVisualModel& visualModel)
{
  if (!m_model) {
    qWarning() << "Cannot load robot visuals: RobotModel is null";
    return false;
  }

  if (m_visualsLoaded) {
    qWarning() << "Robot visuals are already loaded";
    return false;
  }

  for (const RobotLinkVisual& visual : visualModel.linkVisuals) {
    if (visual.linkIndex >= LinkCount) {
      qWarning() << "Invalid robot link index:" << visual.linkIndex;
      return false;
    }

    OccPartOptions options;
    options.transform = identityTransform();
    options.color = visual.color;
    options.selectionMode = OccPart::SelectionMode::None;
    options.showTrihedron = visual.showFrame;
    options.trihedronSize = visual.frameSize;

    const auto id = m_scene.addStepPartWithId(visual.fileName, options);

    if (!id.has_value()) {
      qWarning() << "Cannot load robot link visual:" << visual.fileName;
      return false;
    }

    m_visuals[visual.linkIndex].push_back(RobotVisualInstance{*id, visual.linkToCad});
  }

  m_visualsLoaded = true;
  return true;
}

bool RobotSceneAdapter::applyState(const RobotState& state)
{
  if (!m_model) {
    qWarning() << "Cannot apply robot state: RobotModel is null";
    return false;
  }

  const RobotFrames frames = m_fk.solve(*m_model, state);

  for (std::size_t linkIndex = 0; linkIndex < LinkCount; ++linkIndex) {
    const Transform worldToLink = multiply(m_baseInWorld, frames.baseToLink[linkIndex]);
    for (const RobotVisualInstance& visual : m_visuals[linkIndex]) {
      const Transform worldToCad = multiply(worldToLink, visual.linkToCad);
      if (!m_scene.setPartTransform(visual.partId, worldToCad)) {
        return false;
      }
    }
  }

  m_scene.updateViewer();
  return true;
}

} // namespace robot