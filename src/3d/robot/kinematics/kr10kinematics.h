#pragma once

#include <array>
#include <vector>

#include "3d/mathtypes.h"
#include "3d/robot/model/kr10model.h"

class Kr10Kinematics final
{
public:
  explicit Kr10Kinematics(const Kr10Model& model);

  // Returns absolute base-frame transforms for all robot links.
  [[nodiscard]] std::array<M4d, LinkCount> solveFK(const V6d& q) const;

  // T06 is the flange transform relative to the robot base.
  [[nodiscard]] std::vector<V6d> solveIK(const M4d& T06) const;

  [[nodiscard]] bool isValid() const;

private:
  void addWrist(const M4d& T06, const V6d& q123, std::vector<V6d>& qs) const;
  [[nodiscard]] bool isSolution(const M4d& T06, const V6d& q) const;

private:
  const Kr10Model& m_model;
};