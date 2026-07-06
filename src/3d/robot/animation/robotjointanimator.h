#pragma once

#include "3d/mathtypes.h"

#include <functional>
#include <vector>

#include <QElapsedTimer>
#include <QTimer>

class RobotJointAnimator final
{
public:
  using ApplyStateFn = std::function<bool(const V6d&)>;

public:
  explicit RobotJointAnimator(ApplyStateFn applyState);

  ~RobotJointAnimator() = default;

  RobotJointAnimator(const RobotJointAnimator&) = delete;
  RobotJointAnimator& operator=(const RobotJointAnimator&) = delete;

  RobotJointAnimator(RobotJointAnimator&&) noexcept = delete;
  RobotJointAnimator& operator=(RobotJointAnimator&&) noexcept = delete;

  [[nodiscard]] bool start(std::vector<V6d> qPoints, int segmentDurationMs);

  void stop();

  [[nodiscard]] bool isRunning() const;
  [[nodiscard]] const V6d& currentQ() const;

private:
  void update();

  [[nodiscard]] V6d interpolateCurrentState(std::size_t segmentIndex, double t) const;
  [[nodiscard]] static double smoothStep(double t);

  static constexpr int FrameIntervalMs = 16;

private:
  ApplyStateFn m_applyState;

  QTimer m_timer;
  QElapsedTimer m_clock;

  std::vector<V6d> m_qPoints;
  V6d m_currentQ{};

  int m_segmentDurationMs = 0;
};