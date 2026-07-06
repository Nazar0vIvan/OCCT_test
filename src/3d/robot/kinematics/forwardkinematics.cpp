#include "forwardkinematics.h"

#include <cstddef>

#include "3d/utils.h"

std::array<M4d, LinkCount> ForwardKinematics::linkTransforms(
    const RobotModel &model,
    const std::array<double, DofCount> &q)
{
  std::array<M4d, LinkCount> result{};

  M4d current{};

  result[0] = current;

  for (std::size_t i = 0; i < DofCount; ++i) {
    const JointModel& joint = model.joints[i];

    const double dq = q[i] - model.qHome[i];

    current = current
      .Multiplied(joint.localTransform)
      .Multiplied(makeRotation(dq, joint.axis));

    result[i + 1] = current;
  }

  return result;
}