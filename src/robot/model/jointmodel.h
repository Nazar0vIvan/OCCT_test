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

    // T_parentLink_joint at q = 0.
    Transform parentLinkToJoint = identityTransform();

    // T_joint_childLink at q = 0.
    Transform jointToChildLink = identityTransform();

    double qMin = -std::numeric_limits<double>::infinity();
    double qMax =  std::numeric_limits<double>::infinity();

    double qHome = 0.0;
    double qOffset = 0.0;
  };

}
