#pragma once

#include <array>

#include "3d/robot/model/robotmodel.h"
#include "robotsceneapi.h"

#include <QString>

class RobotSceneAdapter final
{
public:
  RobotSceneAdapter(const RobotModel& model, RobotSceneApi sceneApi);

  using PartId = std::size_t;
  using AddPartFn = std::function<std::optional<PartId>(const LinkModel&)>;
  using SetTransformFn = std::function<bool(PartId, const M4d&)>;
  using UpdateViewerFn = std::function<void()>;

  [[nodiscard]] bool load(const std::array<LinkModel, LinkCount>& links);
  [[nodiscard]] bool applyState(const std::array<double, DofCount>& q);

private:
  RobotModel m_robotmodel;

  std::array<size_t, DofCount> linksPartIds;
  bool m_visualsLoaded = false;
};
