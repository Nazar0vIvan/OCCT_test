#pragma once

#include "3d/mathtypes.h"

namespace robot {

constexpr V6d qHOME = {0.,-90.,90.,0.,0.,0.};

struct RobotState
{
  V6d q{};
  V6d pose{};
};

}
