#pragma once

#include <limits>
#include <string>

#include "3d/mathtypes.h"

enum class JointType { RevoluteZ };

struct JointModel
{
  std::string name;
  JointType type = JointType::RevoluteZ;

  // transform from current i-th JCS to the previous (i-1)-th JSC
  M4d localTransform{};

  double qMin = -std::numeric_limits<double>::infinity();
  double qMax =  std::numeric_limits<double>::infinity();
};


