#pragma once

#include <array>
#include <string>

#include "robot/robottypes.h"
#include "robot/model/jointmodel.h"

namespace robot {

struct RobotModel
{
  std::string name;
  std::array<JointModel, RobotDof> joints;
  Transform link6ToFlange = identityTransform();
  JointArray homeQ{};
};

}
