#pragma once

#include <memory>

#include "3d/robot/model/robotmodel.h"
#include "3d/robot/model/robotstate.h"

#include <QString>

namespace robot {

class RobotSceneAdapter final
{
public:
  RobotSceneAdapter(std::shared_ptr<const RobotModel> model);

  [[nodiscard]] bool load(const RobotModel& visualModel);





  [[nodiscard]] bool applyState(const RobotState& state);

private:
  std::shared_ptr<const RobotModel> m_model;

  // ForwardKinematics m_fk;

  RobotModel m_robotModel;
  bool m_visualsLoaded = false;
};

} // namespace robot
