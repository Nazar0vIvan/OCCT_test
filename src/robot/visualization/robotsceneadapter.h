#pragma once

#include <array>
#include <memory>
#include <vector>
#include <optional>

#include "robot/robottypes.h"
#include "robot/kinematics/forwardkinematics.h"
#include "robot/model/robotmodel.h"
#include "robot/model/robotstate.h"
#include "robot/visualization/robotvisualmodel.h"

#include <QString>

namespace robot {

class RobotSceneAdapter final
{
public:
  RobotSceneAdapter(std::shared_ptr<const RobotModel> model, OccScene& scene);

  [[nodiscard]] bool loadVisuals(const RobotVisualModel& visualModel);
  [[nodiscard]] bool applyState(const RobotState& state);

  void setBaseInWorld(const Transform& baseInWorld);

private:
  std::shared_ptr<const RobotModel> m_model;
  OccScene& m_scene;

  Transform m_baseInWorld = identityTransform();

  ForwardKinematics m_fk;

  std::array<std::vector<RobotVisualInstance>, LinkCount> m_visuals;
  bool m_visualsLoaded = false;
};

} // namespace robot
