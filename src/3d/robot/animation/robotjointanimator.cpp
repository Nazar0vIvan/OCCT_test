#include "robotjointanimator.h"

#include <cstddef>

#include <QDebug>
#include <QObject>
#include <Qt>

RobotJointAnimator::RobotJointAnimator(ApplyStateFn applyState)
    : m_applyState(std::move(applyState)),
      m_timer(),
      m_clock()
{
  m_timer.setInterval(FrameIntervalMs);
  m_timer.setTimerType(Qt::PreciseTimer);

  QObject::connect(&m_timer, &QTimer::timeout, [this]() { update(); });
}

bool RobotJointAnimator::start(std::vector<V6d> qPoints, const int segmentDurationMs)
{
  if (!m_applyState) {
    qWarning() << "Cannot start robot animation: apply-state callback is invalid";
    return false;
  }

  if (qPoints.size() < 2) {
    qWarning() << "Cannot start robot animation: at least two q-points are required";
    return false;
  }

  if (segmentDurationMs <= 0) {
    qWarning() << "Cannot start robot animation: segment duration must be positive";
    return false;
  }

  stop();

  m_qPoints = std::move(qPoints);
  m_segmentDurationMs = segmentDurationMs;
  m_currentQ = m_qPoints.front();

  if (!m_applyState(m_currentQ)) {
    qWarning() << "Cannot start robot animation: failed to apply initial state";
    m_qPoints.clear();
    m_segmentDurationMs = 0;
    return false;
  }

  m_clock.restart();
  m_timer.start();

  return true;
}

void RobotJointAnimator::stop()
{
  if (m_timer.isActive()) {
    m_timer.stop();
  }
}

bool RobotJointAnimator::isRunning() const
{
  return m_timer.isActive();
}

const V6d& RobotJointAnimator::currentQ() const
{
  return m_currentQ;
}

void RobotJointAnimator::update()
{
  if (m_qPoints.size() < 2 || m_segmentDurationMs <= 0) {
    stop();
    return;
  }

  const qint64 elapsedMs = m_clock.elapsed();

  const qint64 segmentDurationMs = static_cast<qint64>(m_segmentDurationMs);

  const qint64 segmentCount = static_cast<qint64>(m_qPoints.size() - 1);

  const qint64 totalDurationMs =  segmentDurationMs * segmentCount;

  if (elapsedMs >= totalDurationMs) {
    m_currentQ = m_qPoints.back();

    if (!m_applyState(m_currentQ)) {
      qWarning() << "Cannot apply final robot animation state";
    }

    stop();
    return;
  }

  const qint64 rawSegmentIndex = elapsedMs / segmentDurationMs;

  const std::size_t segmentIndex = static_cast<std::size_t>(rawSegmentIndex);

  const qint64 localElapsedMs = elapsedMs - rawSegmentIndex * segmentDurationMs;

  const double t = static_cast<double>(localElapsedMs) / static_cast<double>(segmentDurationMs);

  m_currentQ = interpolateCurrentState(segmentIndex, smoothStep(t));

  if (!m_applyState(m_currentQ)) {
    qWarning() << "Cannot apply robot animation state";
    stop();
  }
}

V6d RobotJointAnimator::interpolateCurrentState(const std::size_t segmentIndex, const double t) const
{
  V6d q{};
  const V6d& from = m_qPoints[segmentIndex];
  const V6d& to = m_qPoints[segmentIndex + 1];

  for (std::size_t i = 0; i < q.size(); ++i) {
    q[i] = from[i] + (to[i] - from[i]) * t;
  }

  return q;
}

double RobotJointAnimator::smoothStep(const double t)
{
  if (t <= 0.0) return 0.0;
  if (t >= 1.0) return 1.0;
  return t * t * (3.0 - 2.0 * t);
}