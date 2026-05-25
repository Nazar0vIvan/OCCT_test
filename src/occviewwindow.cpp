#include "occviewwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDir>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_SelectionScheme.hxx>

#include <Aspect_DisplayConnection.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepMesh_IncrementalMesh.hxx>


#include <Graphic3d_Camera.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_Vec.hxx>

#include <IFSelect_ReturnStatus.hxx>

#include <OpenGl_GraphicDriver.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TypeOfHighlight.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_DatumParts.hxx>

#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <STEPControl_Reader.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

#include <V3d_Trihedron.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#include <AIS_Trihedron.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Line.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <TCollection_ExtendedString.hxx>


#include <WNT_Window.hxx>

OccViewWindow::OccViewWindow(QWindow* parent) : QWindow(parent)
{
  setSurfaceType(QSurface::OpenGLSurface);
}

// EVENTS
void OccViewWindow::exposeEvent(QExposeEvent* event)
{
  Q_UNUSED(event);

  if (!isExposed()) return;

  initializeSceneIfNeeded();
  redraw();
}

void OccViewWindow::resizeEvent(QResizeEvent* event)
{
  Q_UNUSED(event);

  if (!m_view.IsNull()) {
    m_view->MustBeResized();
    redraw();
  }
}

void OccViewWindow::mousePressEvent(QMouseEvent* event)
{
  if (m_view.IsNull()) return;

  m_lastMousePos = event->position().toPoint();

  if (event->button() == Qt::LeftButton) {
    m_context->MoveTo(
        m_lastMousePos.x(),
        m_lastMousePos.y(),
        m_view,
        Standard_False
        );

    if (m_context->HasDetected()) {
      Handle(AIS_ViewCubeOwner) cubeOwner = Handle(AIS_ViewCubeOwner)::DownCast(m_context->DetectedOwner());

      if (!cubeOwner.IsNull()) {
        m_view->SetProj(cubeOwner->MainOrientation());

        m_view->ZFitAll();
        updateWorldCoordinateSegments();
        updateViewerAndRedraw();

        event->accept();
        return;
      }

      m_context->SelectDetected(AIS_SelectionScheme_Replace);
    } else {
      m_context->ClearSelected(Standard_False);
    }

    updateViewerAndRedraw();
    event->accept();
    return;
  }

  if (event->button() == Qt::RightButton) {
    m_rotating = true;
    m_view->StartRotation(m_lastMousePos.x(), m_lastMousePos.y());
    event->accept();
    return;
  }

  if (event->button() == Qt::MiddleButton) {
    m_panning = true;
    event->accept();
    return;
  }
}

void OccViewWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (m_view.IsNull()) return;

  const QPoint pos = event->position().toPoint();

  if (m_rotating) {
    m_view->Rotation(pos.x(), pos.y());
    redraw();
    event->accept();
    return;
  }

  if (m_panning) {
    const QPoint delta = pos - m_lastMousePos;

    m_view->Pan(delta.x(), -delta.y());
    redraw();

    m_lastMousePos = pos;

    event->accept();
    return;
  }

  m_context->MoveTo(
    pos.x(),
    pos.y(),
    m_view,
    Standard_True
  );

  event->accept();
}

void OccViewWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton) {
    m_rotating = false;
    event->accept();
    return;
  }

  if (event->button() == Qt::MiddleButton) {
    m_panning = false;
    event->accept();
    return;
  }
}

void OccViewWindow::wheelEvent(QWheelEvent* event)
{
  if (m_view.IsNull()) {
    return;
  }

  const QPoint pos = event->position().toPoint();

  if (event->angleDelta().y() > 0) {
    m_view->Zoom(pos.x(), pos.y(), pos.x() + 10, pos.y() + 10);
  } else {
    m_view->Zoom(pos.x(), pos.y(), pos.x() - 10, pos.y() - 10);
  }

  updateWorldCoordinateSegments();

  updateViewerAndRedraw();

  event->accept();
}

// METHODS
void OccViewWindow::initializeSceneIfNeeded()
{
  if (m_initialized)  return;

  m_displayConnection = new Aspect_DisplayConnection();
  m_graphicDriver = new OpenGl_GraphicDriver(m_displayConnection);

  m_viewer = new V3d_Viewer(m_graphicDriver);
  m_viewer->SetDefaultLights();
  m_viewer->SetLightOn();

  m_context = new AIS_InteractiveContext(m_viewer);

  m_context->SetAutomaticHilight(Standard_True);
  m_context->SetPixelTolerance(5);

  Handle(Prs3d_Drawer) hoverStyle = m_context->HighlightStyle(Prs3d_TypeOfHighlight_LocalDynamic);

  hoverStyle->SetColor(rgb(102, 179, 204));
  hoverStyle->SetDisplayMode(AIS_Shaded);
  hoverStyle->SetFaceBoundaryDraw(Standard_False);
  hoverStyle->SetZLayer(Graphic3d_ZLayerId_Top);

  Handle(Prs3d_Drawer) selectionStyle = m_context->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected);

  selectionStyle->SetColor(rgb(0, 128, 255));
  selectionStyle->SetDisplayMode(AIS_Shaded);
  selectionStyle->SetFaceBoundaryDraw(Standard_False);
  selectionStyle->SetZLayer(Graphic3d_ZLayerId_Top);

  m_view = m_viewer->CreateView();

  Handle(WNT_Window) occWindow = new WNT_Window(reinterpret_cast<Aspect_Handle>(winId()));

  m_view->SetWindow(occWindow);

  if (!occWindow->IsMapped()) occWindow->Map();

  m_view->SetBackgroundColor(Quantity_Color(0.85, 0.85, 0.85, Quantity_TOC_RGB));
  m_view->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
  m_view->MustBeResized();

  Graphic3d_RenderingParams& params = m_view->ChangeRenderingParams();

  params.NbMsaaSamples = 4;
  params.ShadingModel = Graphic3d_TypeOfShadingModel_Phong;
  params.LineFeather = 1.2f;

  createViewCube();

  loadDefaultScene();

  m_view->FitAll();
  m_view->ZFitAll();

  createWorldCoordinateSegments();

  m_context->UpdateCurrentViewer();

  m_initialized = true;
}

void OccViewWindow::loadDefaultScene()
{
  const QDir cadDir(QStringLiteral(OCCT_TEST_CAD_DIR));

  importStepFile(
    cadDir.filePath(QStringLiteral("0_BASE.STEP")),
    rgb(180, 200, 230)
  );
}

Handle(AIS_Shape) OccViewWindow::createConfiguredShape(const TopoDS_Shape& shape, const Quantity_Color& color)
{
  Handle(AIS_Shape) aisShape = new AIS_Shape(shape);

  aisShape->SetDisplayMode(AIS_Shaded);
  aisShape->SetMaterial(Graphic3d_NOM_SATIN);
  aisShape->SetColor(color);

  aisShape->Attributes()->SetFaceBoundaryDraw(Standard_True);
  aisShape->Attributes()->SetFaceBoundaryAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.5));

  return aisShape;
}

void OccViewWindow::displayConfiguredShape(const Handle(AIS_Shape)& aisShape)
{
  if (aisShape.IsNull()) return;

  m_context->Display(aisShape, AIS_Shaded, -1, Standard_False);

  m_context->SetSelectionModeActive(
    aisShape,
    AIS_Shape::SelectionMode(TopAbs_FACE),
    Standard_True,
    AIS_SelectionModesConcurrency_GlobalOrLocal
  );

  m_context->SetSelectionModeActive(
    aisShape,
    AIS_Shape::SelectionMode(TopAbs_EDGE),
    Standard_True,
    AIS_SelectionModesConcurrency_GlobalOrLocal
  );

  m_context->SetSelectionModeActive(
    aisShape,
    AIS_Shape::SelectionMode(TopAbs_VERTEX),
    Standard_True,
    AIS_SelectionModesConcurrency_GlobalOrLocal
  );
}

void OccViewWindow::createWorldCoordinateSegments()
{
  const Standard_Real length = currentWcsAxisLength();
  const Standard_Real width = 2.0;

  m_originPoint = new Geom_CartesianPoint(0.0, 0.0, 0.0);

  m_xEndPoint = new Geom_CartesianPoint(length, 0.0, 0.0);
  m_yEndPoint = new Geom_CartesianPoint(0.0, length, 0.0);
  m_zEndPoint = new Geom_CartesianPoint(0.0, 0.0, length);

  m_xAxis = new AIS_Line(m_originPoint, m_xEndPoint);
  m_yAxis = new AIS_Line(m_originPoint, m_yEndPoint);
  m_zAxis = new AIS_Line(m_originPoint, m_zEndPoint);

  m_xAxis->SetColor(rgb(255, 0, 0));
  m_yAxis->SetColor(rgb(0, 180, 0));
  m_zAxis->SetColor(rgb(0, 0, 255));

  m_xAxis->SetWidth(width);
  m_yAxis->SetWidth(width);
  m_zAxis->SetWidth(width);

  m_xAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);
  m_yAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);
  m_zAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);

  m_context->Display(m_xAxis, Standard_False);
  m_context->Display(m_yAxis, Standard_False);
  m_context->Display(m_zAxis, Standard_False);

  // Passive reference axes: not selectable.
  m_context->Deactivate(m_xAxis);
  m_context->Deactivate(m_yAxis);
  m_context->Deactivate(m_zAxis);
}

void OccViewWindow::updateWorldCoordinateSegments()
{
  if (m_context.IsNull() || m_view.IsNull()) {
    return;
  }

  if (m_xAxis.IsNull() || m_yAxis.IsNull() || m_zAxis.IsNull()) {
    return;
  }

  const Standard_Real length = currentWcsAxisLength();

  m_xEndPoint->SetCoord(length, 0.0, 0.0);
  m_yEndPoint->SetCoord(0.0, length, 0.0);
  m_zEndPoint->SetCoord(0.0, 0.0, length);

  m_context->Redisplay(m_xAxis, Standard_False);
  m_context->Redisplay(m_yAxis, Standard_False);
  m_context->Redisplay(m_zAxis, Standard_False);
}

Standard_Real OccViewWindow::currentWcsAxisLength() const
{
  if (m_view.IsNull()) return 10.0;

  return m_view->Camera()->Scale() * 0.05;
}

void OccViewWindow::createViewCube()
{
  if (!m_viewCube.IsNull()) return;

  m_viewCube = new AIS_ViewCube();

  m_viewCube->SetSize(50.0, Standard_False);

  m_viewCube->SetBoxSideLabel(V3d_Zpos, "TOP");
  m_viewCube->SetBoxSideLabel(V3d_Zneg, "BOTTOM");
  m_viewCube->SetBoxSideLabel(V3d_Yneg, "FRONT");
  m_viewCube->SetBoxSideLabel(V3d_Ypos, "BACK");
  m_viewCube->SetBoxSideLabel(V3d_Xpos, "RIGHT");
  m_viewCube->SetBoxSideLabel(V3d_Xneg, "LEFT");

  m_viewCube->Attributes()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  m_viewCube->BoxSideStyle()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  m_viewCube->BoxEdgeStyle()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  m_viewCube->BoxCornerStyle()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

  m_viewCube->SetBoxColor(rgb(200, 200, 230));
  m_viewCube->SetInnerColor(rgb(0, 0, 0));
  m_viewCube->SetBoxTransparency(0.0);

  Handle(Prs3d_Drawer) cubeHoverStyle = m_viewCube->DynamicHilightAttributes();

  if (!cubeHoverStyle.IsNull()) {
    const Quantity_Color hoverColor = rgb(102, 179, 204);

    cubeHoverStyle->SetColor(hoverColor);

    if (!cubeHoverStyle->ShadingAspect().IsNull()) {
      cubeHoverStyle->ShadingAspect()->SetColor(hoverColor);
    }

    cubeHoverStyle->SetDisplayMode(AIS_Shaded);
    cubeHoverStyle->SetFaceBoundaryDraw(Standard_False);
    cubeHoverStyle->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  }

  m_viewCube->SetTextColor(Quantity_NOC_BLACK);
  m_viewCube->SetFont("Consolas");
  m_viewCube->SetFontHeight(14.0);

  m_viewCube->SetDrawEdges(Standard_True);
  m_viewCube->SetDrawVertices(Standard_True);
  m_viewCube->SetBoxFacetExtension(12.0);

  m_viewCube->BoxEdgeStyle()->SetColor(rgb(150, 150, 180));
  m_viewCube->BoxCornerStyle()->SetColor(rgb(120, 120, 150));

  m_viewCube->SetDrawAxes(Standard_True);
  m_viewCube->SetAxesLabels("", "", "");
  m_viewCube->SetAxesPadding(10.0);
  m_viewCube->SetAxesRadius(1.0);
  m_viewCube->SetAxesConeRadius(0.0);
  m_viewCube->SetAxesSphereRadius(0.0);

  Handle(Prs3d_Drawer) drawer = m_viewCube->Attributes();

  if (drawer.IsNull()) {
    drawer = new Prs3d_Drawer();
    m_viewCube->SetAttributes(drawer);
  }

  Handle(Prs3d_DatumAspect) datumAspect = drawer->DatumAspect();

  if (datumAspect.IsNull()) {
    datumAspect = new Prs3d_DatumAspect();
    drawer->SetDatumAspect(datumAspect);
  }

  datumAspect->ShadingAspect(Prs3d_DP_XAxis)->SetColor(rgb(255, 0, 0));
  datumAspect->ShadingAspect(Prs3d_DP_YAxis)->SetColor(rgb(0, 255, 0));
  datumAspect->ShadingAspect(Prs3d_DP_ZAxis)->SetColor(rgb(0, 0, 255));

  m_viewCube->SetTransformPersistence(
    new Graphic3d_TransformPers(
        Graphic3d_TMF_TriedronPers,
        Aspect_TOTP_LEFT_LOWER,
        Graphic3d_Vec2i(100, 100)
        )
    );

  m_viewCube->SetZLayer(Graphic3d_ZLayerId_Topmost);

  m_viewCube->SetResetCamera(Standard_True);
  m_viewCube->SetFitSelected(Standard_False);

  m_context->Display(m_viewCube, Standard_False);
  m_context->Activate(m_viewCube, 0, Standard_False);
}

void OccViewWindow::redraw()
{
  if (m_view.IsNull()) {
    return;
  }

  if (!m_viewCube.IsNull() && m_viewCube->HasAnimation()) {
    m_viewCube->UpdateAnimation(Standard_True);
  }

  m_view->Redraw();
}

void OccViewWindow::updateViewerAndRedraw()
{
  if (!m_context.IsNull()) {
    m_context->UpdateCurrentViewer();
  }

  redraw();
}

bool OccViewWindow::importStepFile(const QString& filePath, const Quantity_Color& color)
{
  if (m_context.IsNull() || m_view.IsNull()) {
    qWarning() << "OCCT view is not initialized";
    return false;
  }

  if (!QFileInfo::exists(filePath)) {
    qWarning() << "STEP file does not exist:" << filePath;
    return false;
  }

  STEPControl_Reader reader;

  const QByteArray filePathBytes = QDir::toNativeSeparators(filePath).toUtf8();

  const IFSelect_ReturnStatus status =
      reader.ReadFile(filePathBytes.constData());

  if (status != IFSelect_RetDone) {
    qWarning() << "Failed to read STEP file:" << filePath;
    return false;
  }

  const Standard_Integer rootsTransferred = reader.TransferRoots();

  if (rootsTransferred <= 0) {
    qWarning() << "No STEP roots transferred:" << filePath;
    return false;
  }

  TopoDS_Shape shape = reader.OneShape();

  if (shape.IsNull()) {
    qWarning() << "Imported STEP shape is null:" << filePath;
    return false;
  }

  Handle(AIS_Shape) aisShape = createConfiguredShape(shape, color);
  displayConfiguredShape(aisShape);

  return true;
}

Quantity_Color OccViewWindow::rgb(int r, int g, int b)
{
  return Quantity_Color(
    r / 255.0,
    g / 255.0,
    b / 255.0,
    Quantity_TOC_RGB
  );
}

/*
void OccViewWindow::displayTestShape()
{
  if (m_shapeDisplayed) return;

  TopoDS_Shape box = BRepPrimAPI_MakeBox(10., 20., 30.).Shape();
  Handle(AIS_Shape) aisBox = createConfiguredShape(box);
  displayConfiguredShape(aisBox);

  m_view->FitAll();
  m_view->ZFitAll();

  m_context->UpdateCurrentViewer();
  m_view->Redraw();

  m_shapeDisplayed = true;
}
*/

/*
void OccViewWindow::displayTrihedron()
{
  m_view->TriedronDisplay(
    Aspect_TOTP_LEFT_LOWER,
    Quantity_NOC_WHITE, // labels X, Y, Z color
    0.12,               // 12% of window width
    V3d_ZBUFFER
  );

  // Axis
  m_view->ZBufferTriedronSetup(
    Quantity_NOC_RED,    // X axis
    Quantity_NOC_GREEN,  // Y axis
    Quantity_NOC_BLUE1,  // Z axis
    0.8,                 // size ratio
    0.05,                // axis diameter relative to axis length
    12                   // number of facets for cylinders/cones
  );

  // Labels
  const char* fontName = "Calibri";
  const double fontHeight = 14.0;

  for (V3d_TypeOfAxe axis : { V3d_X, V3d_Y, V3d_Z }) {
    Handle(Prs3d_TextAspect) text = m_view->Trihedron()->LabelAspect(axis);

    text->SetFont(fontName);
    text->SetHeight(fontHeight);
    text->Aspect()->SetTextDisplayType(Aspect_TODT_NORMAL);

    if (axis == V3d_X) {
      text->SetColor(Quantity_Color(1.0, 0.0, 0.0, Quantity_TOC_RGB));
    } else if (axis == V3d_Y) {
      text->SetColor(Quantity_Color(0.0, 1.0, 0.0, Quantity_TOC_RGB));
    } else if (axis == V3d_Z) {
      text->SetColor(Quantity_Color(0.0, 0.0, 1.0, Quantity_TOC_RGB));
    }
  }
}
*/

