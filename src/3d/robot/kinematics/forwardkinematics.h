#pragma once

#include <array>

#include "3d/mathtypes.h"
#include "3d/robot/model/robotmodel.h"

class ForwardKinematics final
{
public:
  [[nodiscard]] static std::array<M4d, LinkCount> linkTransforms(
    const RobotModel& model,
    const std::array<double, DofCount>& q);
};