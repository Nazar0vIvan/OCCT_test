#include "robot/visualization/robotsceneadapter.h"

#include <QDebug>

namespace robot {

RobotSceneAdapter::RobotSceneAdapter(
    const RobotModel& model,
    OccScene& scene)
    : m_model(model),
    m_scene(scene),
    m_fk(model)
{
  for (auto& t : m_linkToCad) {
    t.SetIdentity();
  }
}

bool RobotSceneAdapter::loadVisuals(const RobotVisualModel& visualModel)
{
  for (const RobotLinkVisual& visual : visualModel.linkVisuals) {
    if (visual.linkIndex >= LinkCount) {
      qWarning() << "Invalid robot link index:" << visual.linkIndex;
      return false;
    }

    const auto id = m_scene.addStepPartWithId(
        visual.fileName,
        identityTransform(),
        visual.color,
        OccPart::SelectionMode::None,
        visual.showFrame,
        visual.frameSize
        );

    if (!id.has_value()) {
      qWarning() << "Cannot load robot link visual:" << visual.fileName;
      return false;
    }

    m_partIds[visual.linkIndex] = *id;
    m_linkToCad[visual.linkIndex] = visual.linkToCad;
  }

  return true;
}

bool RobotSceneAdapter::applyState(const RobotState& state)
{
  const RobotFrames frames = m_fk.solve(state);

  for (std::size_t i = 0; i < LinkCount; ++i) {
    if (!m_partIds[i].has_value()) {
      continue;
    }

    const Transform worldToCad =
        multiply(
            multiply(m_model.baseInWorld, frames.baseToLink[i]),
            m_linkToCad[i]
            );

    if (!m_scene.setPartTransform(*m_partIds[i], worldToCad)) {
      return false;
    }
  }

  m_scene.updateViewer();
  return true;
}

}
