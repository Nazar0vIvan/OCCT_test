#pragma once

#include <array>

#include "robot/robottypes.h"

namespace robot
{

struct RobotFrames
{
  std::array<Transform, LinkCount> baseToLink{};
  Transform baseToFlange = identityTransform();
};

} // namespace robot
