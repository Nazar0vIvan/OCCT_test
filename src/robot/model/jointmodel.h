#pragma once

#include <limits>
#include <string>

#include "robot/robottypes.h"

namespace robot {

enum class JointType
{
  RevoluteZ
};

struct JointModel
{
  std::string name;
  JointType type = JointType::RevoluteZ;

  Transform Ti = identityTransform(); // transform from current i-th JCS to the previous (i-1)-th JSC

  double qMin = -std::numeric_limits<double>::infinity();
  double qMax =  std::numeric_limits<double>::infinity();

  double qHome = 0.0;
  double qOffset = 0.0;
};

}
