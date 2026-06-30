#pragma once

#include <string>

#include "3d/robot/model/jointmodel.h"
#include "3d/robot/model/linkmodel.h"

constexpr std::size_t DofCount = 6;
constexpr std::size_t LinkCount = DofCount + 1;

struct RobotModel
{
  std::string name;
  std::array<JointModel, DofCount> joints;
  std::array<LinkModel, LinkCount> links;
};


