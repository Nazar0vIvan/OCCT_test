#include "robotmodel.h"

#include <optional>

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include "3d/occt/occutils.h"
#include "3d/utils.h"

static Axis readAxis(const QJsonObject& object)
{
  const QString axis = object.value("axis").toString();

  if (axis == "x" || axis == "X") return Axis::X;
  if (axis == "y" || axis == "Y") return Axis::Y;

  return Axis::Z;
}

static V6d readV6d(const QJsonArray& array)
{
  V6d result{};

  for (std::size_t i = 0; i < DofCount; ++i) {
    result[i] = array.at(static_cast<int>(i)).toDouble();
  }

  return result;
}

static JointModel readJoint(const QJsonObject& object)
{
  const QJsonObject offset = object.value("offset").toObject();

  const double dx = offset.value("dx").toDouble();
  const double dy = offset.value("dy").toDouble();
  const double dz = offset.value("dz").toDouble();

  const double da = offset.value("da").toDouble();
  const double db = offset.value("db").toDouble();
  const double dc = offset.value("dc").toDouble();

  JointModel joint{};

  joint.localTransform =
      makeTranslation(V3d{dx, dy, dz})
      .Multiplied(makeRotation(da, Axis::Z))
      .Multiplied(makeRotation(db, Axis::Y))
      .Multiplied(makeRotation(dc, Axis::X));

  joint.axis = readAxis(object);
  joint.qMin = object.value("min").toDouble();
  joint.qMax = object.value("max").toDouble();

  return joint;
}

static LinkModel readLink(const QJsonObject& object)
{
  const QJsonArray color = object.value("rgb").toArray();

  LinkModel link{};

  link.fileName = object.value("filename").toString();
  link.props.color = rgb(
    color.at(0).toInt(),
    color.at(1).toInt(),
    color.at(2).toInt()
  );

  return link;
}

std::optional<RobotModel> RobotModel::fromFile(const QString& filename)
{
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open robot model file:" << filename;
    return std::nullopt;
  }

  QJsonParseError parseError;
  const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qWarning() << "Invalid robot model JSON:" << parseError.errorString();
    return std::nullopt;
  }

  if (!document.isObject()) {
    qWarning() << "Robot model JSON root must be an object";
    return std::nullopt;
  }

  const QJsonObject root = document.object();

  const QJsonArray joints = root.value("joints").toArray();
  const QJsonArray links = root.value("links").toArray();

  RobotModel model{};

  model.name = root.value("name").toString();
  model.qHome = readV6d(root.value("qHome").toArray());
  model.pHome = readV6d(root.value("pHome").toArray());

  for (std::size_t i = 0; i < DofCount; ++i) {
    model.joints[i] = readJoint(joints.at(static_cast<int>(i)).toObject());
  }

  for (std::size_t i = 0; i < LinkCount; ++i) {
    model.links[i] = readLink(links.at(static_cast<int>(i)).toObject());
  }

  return model;
}

