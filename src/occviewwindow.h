#pragma once

#include <QWindow>
#include <QPoint>

#include <Standard_Handle.hxx>
#include <Quantity_Color.hxx>
#include <vector>
#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>

class QExposeEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;

class Aspect_DisplayConnection;
class OpenGl_GraphicDriver;
class V3d_Viewer;
class V3d_View;
class AIS_InteractiveContext;
class Quantity_Color;
class QString;
class AIS_Shape;
class TopoDS_Shape;
class AIS_Shape;
class AIS_Line;
class Geom_CartesianPoint;
class AIS_ViewCube;
class AIS_Trihedron;

class OccViewWindow : public QWindow
{
  Q_OBJECT

public:
  explicit OccViewWindow(QWindow* parent = nullptr);


  bool importStepFile(
    const QString& filePath,
    const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB)
  );

protected:
  void exposeEvent(QExposeEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  struct ImportedPart
  {
    Handle(AIS_Shape) shape;
    Handle(AIS_Trihedron) trihedron;
    gp_Trsf transform;
  };



  void initializeSceneIfNeeded();
  void loadDefaultScene();

  Handle(AIS_Shape) createConfiguredShape(
    const TopoDS_Shape& shape,
    const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB)
  );
  void displayConfiguredShape(const Handle(AIS_Shape)& aisShape);

  ImportedPart createImportedPart(
    const TopoDS_Shape& shape,
    const Quantity_Color& color
  );

  Handle(AIS_Trihedron) createPartTrihedron(
    const gp_Trsf& filePlacement,
    Standard_Real size
  );

  void displayImportedPart(const ImportedPart& part);

  void setPartTransform(
    ImportedPart& part,
    const gp_Trsf& transform
  );

  void createWorldCoordinateSegments();
  void updateWorldCoordinateSegments();
  Standard_Real currentWcsAxisLength() const;

  void createViewCube();

  void redraw();
  void updateViewerAndRedraw();

  Quantity_Color rgb(int r, int g, int b);

  // void displayTrihedron();
  // void displayTestShape();

private:
  bool m_initialized = false;

  QPoint m_lastMousePos;
  bool m_rotating = false;
  bool m_panning = false;

  Handle(Aspect_DisplayConnection) m_displayConnection;
  Handle(OpenGl_GraphicDriver) m_graphicDriver;
  Handle(V3d_Viewer) m_viewer;
  Handle(AIS_InteractiveContext) m_context;
  Handle(V3d_View) m_view;

  std::vector<ImportedPart> m_parts;

  Handle(AIS_Line) m_xAxis;
  Handle(AIS_Line) m_yAxis;
  Handle(AIS_Line) m_zAxis;

  Handle(Geom_CartesianPoint) m_originPoint;
  Handle(Geom_CartesianPoint) m_xEndPoint;
  Handle(Geom_CartesianPoint) m_yEndPoint;
  Handle(Geom_CartesianPoint) m_zEndPoint;

  Handle(AIS_ViewCube) m_viewCube;
};
