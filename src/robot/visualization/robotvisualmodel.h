#pragma once

#include <cstddef>
#include <vector>

#include <QString>
#include <Quantity_Color.hxx>

#include "robot/robottypes.h"
#include "3d/occutils.h"

namespace robot {

struct RobotLinkVisual
{
  std::size_t linkIndex = 0;
  QString fileName;
  Transform linkToCad = identityTransform();
  Quantity_Color color = kDefaultPartColor;
  bool showFrame = true;
  double frameSize = 60.0;
};

struct RobotVisualModel
{
  std::vector<RobotLinkVisual> linkVisuals;
};

}
