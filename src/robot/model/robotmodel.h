#pragma once

#include <array>
#include <string>

#include "robot/robottypes.h"
#include "robot/model/jointmodel.h"
#include "robot/model/linkmodel.h"

namespace robot {

struct RobotModel
{
  std::string name;

  // links[0] = robot base.
  // links[1..6] = moving links.
  std::array<LinkModel, LinkCount> links;

  // joints[0] connects link 0 -> link 1.
  // joints[1] connects link 1 -> link 2.
  // ...
  // joints[5] connects link 5 -> link 6.
  std::array<JointModel, RobotDof> joints;

  // T_link6_flange.
  Transform link6ToFlange = identityTransform();

  JointArray homeQ{};
};

}
