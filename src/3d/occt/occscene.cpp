#include "occscene.h"

#include <QDir>
#include <QElapsedTimer>
#include <QDebug>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Line.hxx>
#include <AIS_SelectionModesConcurrency.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_ViewCube.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

#include <V3d_View.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#include <Graphic3d_ZLayerId.hxx>

#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include <cmath>

namespace
{
constexpr double PI = 3.141592653589793238462643383279502884;
}

OccScene::OccScene(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view, const QString& cadDir)
  : m_context(context),
    m_view(view),
    m_shapeLoader(cadDir),
    m_worldAxes(currentWorldAxisLength()),
    m_viewCube()
{}

bool OccScene::isValid() const
{
  return !m_context.IsNull() && !m_view.IsNull();
}

bool OccScene::loadStaticScene()
{
  if (!isValid()) {
    qWarning() << "Cannot load static OCCT scene: context or view is null";
    return false;
  }

  displayInfrastructure();

  bool ok = true;

  // ---------------------------------------------------------------------------
  // ROBOT
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // TABLE
  // ---------------------------------------------------------------------------

  TopoDS_Shape tableShape = BRepPrimAPI_MakeBox(740.0, 940.0, 20.0).Shape();

  OccPartOptions tableOptions;
  tableOptions.transform.SetTranslation(gp_Vec(900.0, -470.0, 255.0));

  ok &= addShapePartWithId(tableShape, tableOptions).has_value();

  // ---------------------------------------------------------------------------
  // ROLLER
  // ---------------------------------------------------------------------------

  // const V3d ur(0.999349, -0.036055, 0.000879);
  // const V3d Cr(926.290032, -59.007181, 623.760314); // A point on the axis (near the data “middle”)
  // const double Rr = 20.043646;

  m_context->UpdateCurrentViewer();

  return ok;
}

void OccScene::updateCameraDependentObjects()
{
  if (!isValid()) return;

  if (!m_worldAxesDisplayed) {
    displayWorldAxes();
    return;
  }

  m_worldAxes.setLength(currentWorldAxisLength());
  redisplayWorldAxes();
}

void OccScene::displayInfrastructure()
{
  displayViewCube();
  displayWorldAxes();
}

void OccScene::displayWorldAxes()
{
  if (!isValid()) return;

  if (m_worldAxesDisplayed) return;

  if (!m_worldAxes.isValid()) {
    qWarning() << "Cannot display world axes: OccWorldAxes is invalid";
    return;
  }

  m_worldAxes.setLength(currentWorldAxisLength());

  m_context->Display(m_worldAxes.xAxis(), false);
  m_context->Display(m_worldAxes.yAxis(), false);
  m_context->Display(m_worldAxes.zAxis(), false);

  m_context->Deactivate(m_worldAxes.xAxis());
  m_context->Deactivate(m_worldAxes.yAxis());
  m_context->Deactivate(m_worldAxes.zAxis());

  m_worldAxesDisplayed = true;
}

void OccScene::redisplayWorldAxes()
{
  if (!isValid()) return;

  if (!m_worldAxesDisplayed) return;

  if (!m_worldAxes.isValid()) return;

  m_context->Redisplay(m_worldAxes.xAxis(), false);
  m_context->Redisplay(m_worldAxes.yAxis(), false);
  m_context->Redisplay(m_worldAxes.zAxis(), false);
}

void OccScene::displayViewCube()
{
  if (!isValid()) return;

  if (m_viewCubeDisplayed) return;

  if (!m_viewCube.isValid()) {
    qWarning() << "Cannot display ViewCube: OccViewCube is invalid";
    return;
  }

  m_context->Display(m_viewCube.handle(), false);
  m_context->Activate(m_viewCube.handle(), 0, false);

  m_viewCubeDisplayed = true;
}

bool OccScene::displayPart(OccPart& part)
{
  if (!isValid()) return false;

  if (!part.isValid()) return false;

  const int selectionMode = part.selectionMode() == OccSelectionMode::None ? -1 : 0;

  m_context->Display(part.handle(), AIS_Shaded, selectionMode, false);

  if (part.selectionMode() == OccSelectionMode::All) {
    activateAllSelectionModes(part);
  }

  if (part.hasTrihedron()) {
    m_context->Display(part.trihedron(), false);
    m_context->SetZLayer(part.trihedron(), Graphic3d_ZLayerId_Topmost);
    m_context->Deactivate(part.trihedron());
  }

  return true;
}

void OccScene::activateAllSelectionModes(const OccPart& part)
{
  if (!isValid()) return;

  if (!part.isValid()) return;

  m_context->SetSelectionModeActive(
    part.handle(),
    AIS_Shape::SelectionMode(TopAbs_FACE),
    true,
    AIS_SelectionModesConcurrency_Multiple
  );

  m_context->SetSelectionModeActive(
    part.handle(),
    AIS_Shape::SelectionMode(TopAbs_EDGE),
    true,
    AIS_SelectionModesConcurrency_Multiple
  );

  m_context->SetSelectionModeActive(
    part.handle(),
    AIS_Shape::SelectionMode(TopAbs_VERTEX),
    true,
    AIS_SelectionModesConcurrency_Multiple
  );
}

double OccScene::currentWorldAxisLength() const
{
  if (m_view.IsNull() || m_view->Camera().IsNull()) {
    return 10.0;
  }

  const double cameraScale = m_view->Camera()->Scale();

  if (cameraScale <= 0.0) {
    return 10.0;
  }

  return cameraScale * 0.05;
}

std::optional<OccScene::PartId> OccScene::addStepPartWithId(const QString& stpFileName, const OccPartOptions& options)
{
  if (!isValid()) {
    qWarning() << "Cannot add STEP part: context or view is null";
    return std::nullopt;
  }

  const CachedShapeResult result = m_shapeLoader.loadStpWithCache(stpFileName);

  if (!result.ok) {
    qWarning() << result.error;
    return std::nullopt;
  }

  return addShapePartWithId(result.shape, options);
}

std::optional<OccScene::PartId> OccScene::addShapePartWithId(const TopoDS_Shape& shape, const OccPartOptions& options)
{
  if (!isValid()) {
    qWarning() << "Cannot add OCCT shape part: context or view is null";
    return std::nullopt;
  }

  if (shape.IsNull()) {
    qWarning() << "Cannot add OCCT shape part: TopoDS_Shape is null";
    return std::nullopt;
  }

  OccPart part(shape, options);

  if (!part.isValid()) {
    qWarning() << "Cannot add OCCT shape part: AIS presentation was not created";
    return std::nullopt;
  }

  if (!displayPart(part)) {
    return std::nullopt;
  }

  const PartId id{m_parts.size()};
  m_parts.emplace_back(std::move(part));

  return id;
}

bool OccScene::setPartTransform(const PartId id, const gp_Trsf& transform)
{
  if (id.value >= m_parts.size()) {
    qWarning() << "Invalid OccScene part id:" << id.value;
    return false;
  }

  m_parts[id.value].setTransform(transform);
  return true;
}

void OccScene::updateViewer()
{
  if (!m_context.IsNull()) {
    m_context->UpdateCurrentViewer();
  }
}
