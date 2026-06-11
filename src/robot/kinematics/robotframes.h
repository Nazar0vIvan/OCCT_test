#pragma once

#include <array>

#include "robot/robottypes.h"

namespace robot
{

struct RobotFrames
{
  // T_base_link[i].
  // baseToLink[0] is identity.
  std::array<Transform, LinkCount> baseToLink{};

  // T_base_flange.
  Transform baseToFlange = identityTransform();
};

} // namespace robot
