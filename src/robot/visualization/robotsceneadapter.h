#pragma once

#include <array>
#include <optional>

#include "3d/occscene.h"

#include "robot/robottypes.h"
#include "robot/model/robotmodel.h"
#include "robot/model/robotstate.h"
#include "robot/kinematics/forwardkinematics.h"
#include "robot/visualization/robotvisualmodel.h"

namespace robot {

class RobotSceneAdapter final
{
public:
  RobotSceneAdapter(
      const RobotModel& model,
      OccScene& scene
      );

  bool loadVisuals(const RobotVisualModel& visualModel);

  bool applyState(const RobotState& state);

private:
  const RobotModel& m_model;
  OccScene& m_scene;

  ForwardKinematics m_fk;

  std::array<std::optional<OccScene::PartId>, LinkCount> m_partIds;
  std::array<Transform, LinkCount> m_linkToCad;
};

}
