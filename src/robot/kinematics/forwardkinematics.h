#pragma once

#include "robot/kinematics/robotframes.h"

namespace robot
{

struct RobotModel;
struct RobotState;

class ForwardKinematics final
{
public:
  [[nodiscard]] RobotFrames solve(const RobotModel& model, const RobotState& state) const;
};

}
