#pragma once

#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>

#include <Quantity_Color.hxx>

#include <gp_Trsf.hxx>

class OccPart final
{
public:
  enum class SelectionMode { PartOnly, All, None };

  explicit OccPart(
    const TopoDS_Shape& shape,
    const gp_Trsf& transform = gp_Trsf(),
    const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB),
    SelectionMode selectionMode = SelectionMode::PartOnly
  );

  ~OccPart() = default;

  OccPart(const OccPart&) = delete;
  OccPart& operator=(const OccPart&) = delete;

  OccPart(OccPart&&) noexcept = default;
  OccPart& operator=(OccPart&&) noexcept = default;

  bool isValid() const;

  const Handle(AIS_Shape)& handle() const;

  const Handle(AIS_Trihedron)& trihedron() const;

  const Quantity_Color& color() const;
  void setColor(const Quantity_Color& color);

  const gp_Trsf& transform() const;
  void setTransform(const gp_Trsf& transform);

  SelectionMode selectionMode() const;
  void setSelectionMode(SelectionMode selectionMode);

  bool hasTrihedron() const;
  void enableTrihedron(double size = 20.0);
  void disableTrihedron();

private:
  void createPresentation(const TopoDS_Shape& shape);
  void configureBasePresentation();
  void configureFaceBoundary();

  void applyColor();
  void applyTransform();

  void rebuildTrihedron();
  void configureTrihedron();

private:
  Handle(AIS_Shape) m_handle;
  Handle(AIS_Trihedron) m_trihedron;

  Quantity_Color m_color;
  gp_Trsf m_transform;

  SelectionMode m_selectionMode = SelectionMode::None;

  bool m_trihedronEnabled = false;
  double m_trihedronSize = 20.0;

};
