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

#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <cmath>


OccScene::OccScene(const Handle(AIS_InteractiveContext)& context,const Handle(V3d_View)& view, const QString& cadDirectory)
  : m_context(context),
    m_view(view),
    m_shapeLoader(cadDirectory),
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

  // 0
  ok = addStepPart(QStringLiteral("0_BASE.stp")) && ok;

  // 1
  gp_Trsf trl1;
  trl1.SetTranslation(gp_Vec(0.0, 0.0, 227.0));

  ok = addStepPart(QStringLiteral("1_ROTATING_COLUMN.stp"), trl1) && ok;

  // 2
  gp_Trsf rotX2;
  rotX2.SetRotation(
    gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), -M_PI / 2.0);
  gp_Trsf trl2;
  trl2.SetTranslation(
    gp_Vec(150.0, -90.4, 450.0)
  );

  gp_Trsf trf2 = trl2 * rotX2;

  ok = addStepPart(QStringLiteral("2_LINK_ARM.stp"), trf2) && ok;

  // 3
  gp_Trsf rot3X;
  rot3X.SetRotation(
    gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), -M_PI / 2.0);

  gp_Trsf rot3Y;
  rot3Y.SetRotation(
      gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), M_PI / 2.0);

  gp_Trsf trl3;
  trl3.SetTranslation(gp_Vec(150.0, -101.5, 1060));

  gp_Trsf trf3 = trl3 * rot3Y * rot3X;
  ok = addStepPart(QStringLiteral("3_IN-LINE_WRIST.stp"), trf3) && ok;

  // 4
  gp_Trsf rot4Y;
  rot4Y.SetRotation(
    gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), -M_PI / 2.0);

  gp_Trsf rot4X;
  rot4X.SetRotation(
      gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), M_PI);

  gp_Trsf trl4;
  trl4.SetTranslation(gp_Vec(507.0, 0.0, 1080.0));

  gp_Trsf trf4 = trl4 * rot4X * rot4Y;

  ok = addStepPart(QStringLiteral("4_WRIST1.stp"), trf4) && ok;

  // 5
  gp_Trsf rot5Y;
  rot5Y.SetRotation(
    gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), M_PI / 2.0);

  gp_Trsf rot5Z;
  rot5Z.SetRotation(
      gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 2.0);

  gp_Trsf trl5;
  trl5.SetTranslation(gp_Vec(810.0, 39.0, 1080));

  gp_Trsf trf5 = trl5 * rot5Z * rot5Y;

  ok = addStepPart(QStringLiteral("5_WRIST2.stp"), trf5) && ok;

  // 6
  gp_Trsf rot6Y;
  rot6Y.SetRotation(
    gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), -M_PI / 2.0);

  gp_Trsf rot6X;
  rot6X.SetRotation(
      gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), M_PI);

  gp_Trsf trl6;
  trl6.SetTranslation(gp_Vec(890.0, 0, 1080));

  gp_Trsf trf6 = trl6 * rot6X * rot6Y;

  ok = addStepPart(QStringLiteral("6_WRIST3.stp"), trf6) && ok;

  // ---------------------------------------------------------------------------
  // TABLE
  // ---------------------------------------------------------------------------

  TopoDS_Shape tableShape = BRepPrimAPI_MakeBox(740.0, 940.0, 20.0).Shape();


  gp_Trsf trf_table;
  trf_table.SetTranslation(gp_Vec(900.0, -470.0, 255.0));

  ok = addShapePart(tableShape, trf_table) && ok;

  // ---------------------------------------------------------------------------
  //
  // ---------------------------------------------------------------------------

  /*
  TopoDS_Shape boxShape =
    BRepPrimAPI_MakeBox(100.0, 50.0, 30.0).Shape();

  gp_Trsf boxTransform;
  boxTransform.SetTranslation(gp_Vec(150.0, 0.0, 0.0));

  ok = addShapePart(
    boxShape,
    rgb(210, 150, 90),
    boxTransform
  ) && ok;
  */

  /*
  TopoDS_Shape sphereShape =
    BRepPrimAPI_MakeSphere(25.0).Shape();

  gp_Trsf sphereTransform;
  sphereTransform.SetTranslation(gp_Vec(0.0, 120.0, 0.0));

  ok = addShapePart(
    sphereShape,
    rgb(90, 130, 220),
    sphereTransform
  ) && ok;
  */

  m_context->UpdateCurrentViewer();

  return ok;
}

bool OccScene::addStepPart(
  const QString& stpFileName,
  const gp_Trsf& transform,
  const Quantity_Color& color,
  OccPart::SelectionMode selectionMode)
{
  if (!isValid()) {
    qWarning() << "Cannot add STEP part: context or view is null";
    return false;
  }

  const CachedShapeResult result = m_shapeLoader.loadStpWithCache(stpFileName);

  if (!result.ok) {
    qWarning() << result.error;
    return false;
  }

  return addShapePart(
    result.shape,
    transform,
    color,
    selectionMode
  );
}

bool OccScene::addShapePart(
  const TopoDS_Shape& shape,
  const gp_Trsf& transform,
  const Quantity_Color& color,
  const OccPart::SelectionMode selectionMode)
{
  if (!isValid()) {
    qWarning() << "Cannot add OCCT shape part: context or view is null";
    return false;
  }

  if (shape.IsNull()) {
    qWarning() << "Cannot add OCCT shape part: TopoDS_Shape is null";
    return false;
  }

  QElapsedTimer timer;

  timer.start();
  OccPart part(shape, transform, color, selectionMode);
  qDebug() << "OccPart construction ms:" << timer.elapsed();

  if (!part.isValid()) {
    qWarning() << "Cannot add OCCT shape part: AIS presentation was not created";
    return false;
  }

  timer.restart();
  if (!displayPart(part)) {
    return false;
  }
  qDebug() << "Display part ms:" << timer.elapsed();

  m_parts.emplace_back(std::move(part));

  return true;
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

  // Passive reference axes: not selectable.
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

  const int selectionMode = part.selectionMode() == OccPart::SelectionMode::None ? -1 : 0;

  m_context->Display(
    part.handle(),
    AIS_Shaded,
    selectionMode,
    false
  );

  if (part.selectionMode() == OccPart::SelectionMode::All) {
    activateAllSelectionModes(part);
  }

  if (part.hasTrihedron()) {
    m_context->Display(part.trihedron(), false);
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

Quantity_Color OccScene::rgb(const int r, const int g, const int b)
{
  return Quantity_Color(
    static_cast<double>(r) / 255.0,
    static_cast<double>(g) / 255.0,
    static_cast<double>(b) / 255.0,
    Quantity_TOC_RGB
  );
}
