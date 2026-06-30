#pragma once

#include <string>

#include "3d/robot/model/jointmodel.h"

namespace robot {

constexpr std::size_t RobotDof = 6;
constexpr std::size_t LinkCount = RobotDof + 1;

struct RobotModel
{
  std::string name;
  std::array<JointModel, RobotDof> joints;
};

}
