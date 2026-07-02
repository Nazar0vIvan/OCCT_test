#pragma once

#include <cstddef>
#include <functional>
#include <optional>

#include "3d/robot/model/linkmodel.h"
#include "3d/mathtypes.h"

struct RobotSceneApi
{
  using PartId = std::size_t;
  using AddVisualFn = std::function<std::optional<PartId>(const LinkModel&)>;
  using SetTransformFn = std::function<bool(PartId, const M4d&)>;
  using UpdateViewerFn = std::function<void()>;

  AddVisualFn addVisual;
  SetTransformFn setTransform;
  UpdateViewerFn updateViewer;

  [[nodiscard]] bool isValid() const
  {
    return addVisual && setTransform;
  }
};
