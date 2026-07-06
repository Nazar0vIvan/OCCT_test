#pragma once

#include <array>
#include <optional>

#include "3d/mathtypes.h"
#include "3d/occt/occscene.h"
#include "3d/robot/model/robotmodel.h"

class RobotOccSceneAdapter final
{
public:
  using PartId = OccScene::PartId;

public:
  explicit RobotOccSceneAdapter(OccScene& scene);

  ~RobotOccSceneAdapter() = default;

  RobotOccSceneAdapter(const RobotOccSceneAdapter&) = delete;
  RobotOccSceneAdapter& operator=(const RobotOccSceneAdapter&) = delete;

  RobotOccSceneAdapter(RobotOccSceneAdapter&&) noexcept = delete;
  RobotOccSceneAdapter& operator=(RobotOccSceneAdapter&&) noexcept = delete;

  [[nodiscard]] bool load(const std::array<LinkModel, LinkCount>& links);
  [[nodiscard]] bool applyTransforms(const std::array<M4d, LinkCount>& transforms);

private:
  [[nodiscard]] bool isReady() const;

private:
  OccScene& m_scene;

  std::array<std::optional<PartId>, LinkCount> m_linkPartIds{};
  bool m_linksLoaded = false;
};