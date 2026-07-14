#include "occcontroller.h"

#include "occviewwindow.h"

#include <cstddef>
#include <optional>

#include <QDebug>
#include <QVariant>

#include "3d/mathtypes.h"

namespace
{

std::optional<V6d> readV6d(const QVariantList& vals, const char* op)
{
  V6d out{};

  if (vals.size() != static_cast<int>(out.size())) {
    qWarning() << "Cannot" << op << ": expected 6 values, got" << vals.size();
    return std::nullopt;
  }

  for (std::size_t idx = 0; idx < out.size(); ++idx) {
    bool ok = false;
    const double val = vals.at(static_cast<int>(idx)).toDouble(&ok);

    if (!ok) {
      qWarning() << "Cannot" << op << ": invalid value at index" << idx;
      return std::nullopt;
    }

    out[idx] = val;
  }

  return out;
}

} // namespace
OccController::OccController(QObject* parent) : QObject(parent), m_viewWindow(std::make_unique<OccViewWindow>())
{}

OccController::~OccController()
{
  if (m_viewWindow) {
    m_viewWindow->hide();
  }
}

QWindow* OccController::viewWindow() const
{
  return m_viewWindow.get();
}

void OccController::solveIK(const QVariantList& pose)
{
  const std::optional<V6d> vals = readV6d(pose, "solve IK");
  if (!vals) return;

  m_viewWindow->solveIK(*vals);
}

void OccController::solveFK(const QVariantList& q)
{
  const std::optional<V6d> vals = readV6d(q, "solve FK");
  if (!vals) return;

  m_viewWindow->solveFK(*vals);
}
