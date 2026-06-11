#pragma once

#include <cstddef>
#include <vector>

#include <QString>
#include <Quantity_Color.hxx>

#include "robot/robottypes.h"

namespace robot {

  struct RobotLinkVisual
  {
    std::size_t linkIndex = 0;

    QString fileName;

    // T_link_cad.
    // Fixed offset from mathematical link frame to imported CAD/B-Rep frame.
    Transform linkToCad = identityTransform();

    Quantity_Color color =
        Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB);

    bool showFrame = true;
    double frameSize = 60.0;
  };

  struct RobotVisualModel
  {
    std::vector<RobotLinkVisual> linkVisuals;
  };

}
