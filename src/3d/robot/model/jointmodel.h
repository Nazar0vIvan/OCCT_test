#pragma once

#include <limits>
#include <QString>

#include "3d/mathtypes.h"

struct JointModel
{
  Axis axis = Axis::Z;

  // transform from current i-th JCS to the previous (i-1)-th JSC
  M4d localTransform{};

  double qMin = -std::numeric_limits<double>::infinity();
  double qMax =  std::numeric_limits<double>::infinity();
};


