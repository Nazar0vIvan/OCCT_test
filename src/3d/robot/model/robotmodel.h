#pragma once

#include <array>
#include <cstddef>
#include <optional>

#include <QString>

#include "3d/mathtypes.h"
#include "jointmodel.h"
#include "linkmodel.h"

constexpr std::size_t DofCount = 6;
constexpr std::size_t LinkCount = DofCount + 1;

struct RobotModel
{
  QString name;

  std::array<JointModel, DofCount> joints{};
  std::array<LinkModel, LinkCount> links{};

  V6d qHome{};
  V6d pHome{};

  [[nodiscard]] static std::optional<RobotModel> fromFile(const QString& filename);
};
