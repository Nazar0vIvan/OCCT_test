#pragma once

#include <cstddef>

#include <QString>
#include <Quantity_Color.hxx>

#include "3d/occutils.h"

namespace robot {

struct RobotLinkVisual
{
  std::size_t linkIndex = 0;
  QString fileName;
  OccPartOptions options;
};

struct RobotVisualModel
{
  std::array<RobotLinkVisual, LinkCount> linkVisuals;
};

}
