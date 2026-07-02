#pragma once

#include <QString>

#include "jointmodel.h"
#include "linkmodel.h"

constexpr std::size_t DofCount = 6;
constexpr std::size_t LinkCount = DofCount + 1;

struct RobotModel
{
  static RobotModel fromFile(const QString& filename);

  QString name;
  std::array<JointModel, DofCount> joints;
  std::array<LinkModel, LinkCount> links;

  V6d qHome{0.,0.,0.,0.,0.,0.};
  V6d pHome{0.,0.,0.,0.,0.,0.};
};



inline RobotModel RobotModel::fromFile(const QString &filename)
{

}
