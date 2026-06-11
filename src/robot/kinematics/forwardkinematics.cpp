#include "robot/kinematics/forwardkinematics.h"

#include "robot/model/robotmodel.h"
#include "robot/model/robotstate.h"

namespace robot
{

RobotFrames ForwardKinematics::solve(
    const RobotModel& model,
    const RobotState& state) const
{
  RobotFrames frames;

  for (auto& transform : frames.baseToLink) {
    transform = identityTransform();
  }

  Transform acc = identityTransform();
  frames.baseToLink[0] = acc;

  // TODO:
  // Implement FK here.
  //
  // Intended convention:
  //
  //   acc =
  //       acc
  //     * model.joints[i].parentLinkToJoint
  //     * RotZ(state.q[i] + model.joints[i].qOffset)
  //     * model.joints[i].jointToChildLink;
  //
  //   frames.baseToLink[i + 1] = acc;

  (void)state;

  for (std::size_t i = 0; i < RobotDof; ++i) {
    frames.baseToLink[i + 1] = acc;
  }

  frames.baseToFlange =
      multiply(frames.baseToLink[RobotDof], model.link6ToFlange);

  return frames;
}

} // namespace robot
