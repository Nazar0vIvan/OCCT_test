#pragma once

#include <AIS_Line.hxx>
#include <Geom_CartesianPoint.hxx>

#include <Standard_Real.hxx>

class OccWorldAxes
{
public:
  explicit OccWorldAxes(Standard_Real initialLength = 10.0);

  [[nodiscard]] bool isValid() const;

  [[nodiscard]] Standard_Real length() const;
  void setLength(Standard_Real length);

  [[nodiscard]] const Handle(AIS_Line)& xAxis() const;
  [[nodiscard]] const Handle(AIS_Line)& yAxis() const;
  [[nodiscard]] const Handle(AIS_Line)& zAxis() const;

private:
  void createAxes(Standard_Real initialLength);
  void configureAxes();

  static Quantity_Color rgb(int r, int g, int b);

private:
  Standard_Real m_length = 10.0;

  Handle(Geom_CartesianPoint) m_originPoint;
  Handle(Geom_CartesianPoint) m_xEndPoint;
  Handle(Geom_CartesianPoint) m_yEndPoint;
  Handle(Geom_CartesianPoint) m_zEndPoint;

  Handle(AIS_Line) m_xAxis;
  Handle(AIS_Line) m_yAxis;
  Handle(AIS_Line) m_zAxis;
};
