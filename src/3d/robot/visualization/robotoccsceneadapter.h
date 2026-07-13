#pragma once

#include <array>
#include <optional>

#include "3d/mathtypes.h"
#include "3d/occt/occscene.h"
#include "3d/robot/model/kr10model.h"

class RobotOccSceneAdapter final
{
public:
  using PartId = OccScene::PartId;

  enum class Status{ Done, Failed };

public:
  explicit RobotOccSceneAdapter(OccScene& scene);

  ~RobotOccSceneAdapter() = default;

  RobotOccSceneAdapter(const RobotOccSceneAdapter&) = delete;
  RobotOccSceneAdapter& operator=(const RobotOccSceneAdapter&) = delete;

  RobotOccSceneAdapter(RobotOccSceneAdapter&&) noexcept = delete;
  RobotOccSceneAdapter& operator=(RobotOccSceneAdapter&&) noexcept = delete;

  [[nodiscard]] Status load(const std::array<LinkModel, LinkCount>& links, const LinkModel& endEffector);
  [[nodiscard]] Status applyTransforms(const std::array<M4d, LinkCount>& T0i);

private:
  [[nodiscard]] bool isReady() const;

private:
  OccScene& m_scene;

  std::array<std::optional<PartId>, LinkCount> m_linkIds{};
  std::optional<PartId> m_effPartId;
  bool m_loaded = false;
};