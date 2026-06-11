#pragma once

#include <array>
#include <cstddef>

#include <gp_Trsf.hxx>

namespace robot
{

constexpr std::size_t RobotDof = 6;
constexpr std::size_t LinkCount = RobotDof + 1;

using Transform = gp_Trsf;
using JointArray = std::array<double, RobotDof>;

[[nodiscard]] inline Transform identityTransform()
{
  return Transform{}; // gp_Trsf default constructor creates identity transform
}

[[nodiscard]] inline Transform inverse(Transform transform)
{
  transform.Invert();
  return transform;
}

[[nodiscard]] inline Transform multiply(
    const Transform& lhs,
    const Transform& rhs)
{
  return lhs * rhs;
}

} // namespace robot
