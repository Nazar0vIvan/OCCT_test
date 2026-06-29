#pragma once

#include "robot/robottypes.h"

namespace robot {

  struct RobotState
  {
    JointArray q{};

    [[nodiscard]] static RobotState home(const JointArray& homeQ)
    {
      RobotState state;
      state.q = homeQ;
      return state;
    }
  };

}
