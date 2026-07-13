#include "kr10kinematics.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include <QDebug>

#include "3d/utils.h"

namespace {

struct Q1Branch
{
  double dq1 = 0.0;
  double rho = 0.0;
};

std::vector<double> validAngles(const double angle, const double minDeg, const double maxDeg)
{
  std::vector<double> angles;

  // minDeg <= angle + 360.0 * turn <= maxDeg
  const int firstK = static_cast<int>(std::ceil((minDeg - angle - GeomConst::Eps) / 360.0));
  const int lastK = static_cast<int>(std::floor((maxDeg - angle + GeomConst::Eps) / 360.0));

  for (int k = firstK; k <= lastK; ++k) {
    const double angle2 = angle + 360.0 * static_cast<double>(k);

    if (angle2 >= minDeg - GeomConst::Eps && angle2 <= maxDeg + GeomConst::Eps) {
      angles.push_back(angle2);
    }
  }

  return angles;
}

double posErr(const M4d& T)
{
  return V3d{T.TranslationPart()}.Magnitude();
}

double rotErr(const M4d& T)
{
  double maxErr = 0.0;

  for (int row = 1; row <= 3; ++row) {
    for (int col = 1; col <= 3; ++col) {
      const double want = row == col ? 1.0 : 0.0;
      maxErr = std::max(maxErr, std::abs(T.Value(row, col) - want));
    }
  }

  return maxErr;
}

std::vector<Q1Branch> q1Branches(const V3d& oW)
{
  const double rho = std::hypot(oW.X(), oW.Y());

  if (rho <= GeomConst::Eps) return {{0.0, 0.0}};

  const double q1 = std::atan2(oW.Y(), oW.X());

  return {
    {q1, rho},
    {q1 + GeomConst::Pi, -rho}
  };
}

} // namespace

Kr10Kinematics::Kr10Kinematics(const Kr10Model& model) : m_model(model)
{}

bool Kr10Kinematics::isValid() const
{
  const Kr10Model::IkParams& ik = m_model.ik;
  const double b = std::hypot(ik.by, ik.bx);

  return ik.a > GeomConst::Eps && b > GeomConst::Eps && std::abs(ik.dF) > GeomConst::Eps;
}

std::array<M4d, LinkCount> Kr10Kinematics::solveFK(const V6d& q) const
{
  std::array<M4d, LinkCount> T0i{};

  M4d T{};
  T0i[0] = T;

  for (std::size_t i = 0; i < DofCount; ++i) {
    const JointModel& joint = m_model.joints[i];
    const double dq = q[i] - m_model.qHome[i];

    T = T.Multiplied(joint.localTransform).Multiplied(makeRotation(dq, joint.axis));
    T0i[i + 1] = T;
  }

  return T0i;
}

std::vector<V6d> Kr10Kinematics::solveIK(const M4d& T06) const
{
  if (!isValid()) {
    qWarning() << "Cannot solve KR10 IK: invalid KR10 kinematic parameters";
    return {};
  }

  const Kr10Model::IkParams& ik = m_model.ik;

  const double b = std::hypot(ik.by, ik.bx);
  const double alpha = std::atan2(ik.bx, ik.by);

  const V3d o06 = V3d{T06.TranslationPart()};
  const V3d z06 = V3d{T06.VectorialPart().Column(3)};
  const V3d oW = o06.Subtracted(z06.Multiplied(ik.dF));

  std::vector<V6d> qs;

  for (const Q1Branch& br : q1Branches(oW)) {
    const double px = br.rho - ik.sx;
    const double pz = oW.Z() - ik.sz;
    const double r2 = px * px + pz * pz;

    if (r2 <= GeomConst::Eps * GeomConst::Eps) continue;

    const double ce = (r2 - ik.a * ik.a - b * b) / (2.0 * ik.a * b);

    for (const double el : acos2(ce)) {
      const double sh = std::atan2(pz, px) - std::atan2(b * std::sin(el), ik.a + b * std::cos(el));
      const double dq2 = GeomConst::Pi / 2.0 - sh;
      const double dq3 = alpha - GeomConst::Pi / 2.0 - el;

      const std::vector<double> q1s =
          validAngles(m_model.qHome[0] + br.dq1 * GeomConst::RadToDeg, m_model.joints[0].qMin, m_model.joints[0].qMax);
      const std::vector<double> q2s =
          validAngles(m_model.qHome[1] + dq2 * GeomConst::RadToDeg, m_model.joints[1].qMin, m_model.joints[1].qMax);
      const std::vector<double> q3s =
          validAngles(m_model.qHome[2] + dq3 * GeomConst::RadToDeg, m_model.joints[2].qMin, m_model.joints[2].qMax);

      for (const double q1 : q1s) {
        for (const double q2 : q2s) {
          for (const double q3 : q3s) {
            V6d q123 = m_model.qHome;

            q123[0] = q1;
            q123[1] = q2;
            q123[2] = q3;

            addWrist(T06, q123, qs);
          }
        }
      }
    }
  }

  return qs;
}

void Kr10Kinematics::addWrist(const M4d& T06, const V6d& q123, std::vector<V6d>& qs) const
{
  const std::array<M4d, LinkCount> T0i = solveFK(q123);
  const M4d T36 = T0i[3].Inverted().Multiplied(T06);

  const double cq5 = -T36.Value(2, 3);

  for (const double dq5 : acos2(cq5)) {
    const double sq5 = std::sin(dq5);

    double dq4 = 0.0;
    double dq6 = 0.0;

    if (std::abs(sq5) > GeomConst::Eps) {
      dq4 = std::atan2(-T36.Value(3, 3) / sq5, T36.Value(1, 3) / sq5);
      dq6 = std::atan2(-T36.Value(2, 2) / sq5, T36.Value(2, 1) / sq5);
    } else if (cq5 >= 0.0) {
      // Wrist singularity: q5 is near 0. Return one representative with dq6 = 0.
      dq4 = std::atan2(T36.Value(1, 2), T36.Value(1, 1));
    } else {
      // Wrist singularity: q5 is near pi. Return one representative with dq6 = 0.
      dq4 = std::atan2(T36.Value(1, 2), -T36.Value(1, 1));
    }

    const std::vector<double> q4s = validAngles(m_model.qHome[3] + dq4 * GeomConst::RadToDeg, m_model.joints[3].qMin, m_model.joints[3].qMax);
    const std::vector<double> q5s = validAngles(m_model.qHome[4] + dq5 * GeomConst::RadToDeg, m_model.joints[4].qMin, m_model.joints[4].qMax);
    const std::vector<double> q6s = validAngles(m_model.qHome[5] + dq6 * GeomConst::RadToDeg, m_model.joints[5].qMin, m_model.joints[5].qMax);

    for (const double q4 : q4s) {
      for (const double q5v : q5s) {
        for (const double q6 : q6s) {
          V6d q = q123;

          q[3] = q4;
          q[4] = q5v;
          q[5] = q6;

          if (isSolution(T06, q)) qs.push_back(q);
        }
      }
    }
  }
}

bool Kr10Kinematics::isSolution(const M4d& T06, const V6d& q) const
{
  const std::array<M4d, LinkCount> T0i = solveFK(q);
  const M4d Terr = T06.Inverted().Multiplied(T0i[EndEffIdx]);

  return posErr(Terr) <= GeomConst::PosEps && rotErr(Terr) <= GeomConst::RotEps;
}
