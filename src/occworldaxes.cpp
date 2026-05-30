#include "occworldaxes.h"

#include <AIS_Line.hxx>

#include <Geom_CartesianPoint.hxx>

#include <Graphic3d_ZLayerId.hxx>

#include <Quantity_Color.hxx>

OccWorldAxes::OccWorldAxes(const Standard_Real initialLength)
{
  createAxes(initialLength);
  configureAxes();
}

bool OccWorldAxes::isValid() const
{
  return !m_originPoint.IsNull()
      && !m_xEndPoint.IsNull()
      && !m_yEndPoint.IsNull()
      && !m_zEndPoint.IsNull()
      && !m_xAxis.IsNull()
      && !m_yAxis.IsNull()
      && !m_zAxis.IsNull();
}

Standard_Real OccWorldAxes::length() const
{
  return m_length;
}

void OccWorldAxes::setLength(const Standard_Real length)
{
  if (length <= 0.0) {
    return;
  }

  m_length = length;

  if (m_xEndPoint.IsNull() || m_yEndPoint.IsNull() || m_zEndPoint.IsNull()) {
    return;
  }

  m_xEndPoint->SetCoord(m_length, 0.0, 0.0);
  m_yEndPoint->SetCoord(0.0, m_length, 0.0);
  m_zEndPoint->SetCoord(0.0, 0.0, m_length);
}

const Handle(AIS_Line)& OccWorldAxes::xAxis() const
{
  return m_xAxis;
}

const Handle(AIS_Line)& OccWorldAxes::yAxis() const
{
  return m_yAxis;
}

const Handle(AIS_Line)& OccWorldAxes::zAxis() const
{
  return m_zAxis;
}

void OccWorldAxes::createAxes(const Standard_Real initialLength)
{
  m_length = initialLength > 0.0 ? initialLength : 10.0;

  m_originPoint = new Geom_CartesianPoint(0.0, 0.0, 0.0);

  m_xEndPoint = new Geom_CartesianPoint(m_length, 0.0, 0.0);
  m_yEndPoint = new Geom_CartesianPoint(0.0, m_length, 0.0);
  m_zEndPoint = new Geom_CartesianPoint(0.0, 0.0, m_length);

  m_xAxis = new AIS_Line(m_originPoint, m_xEndPoint);
  m_yAxis = new AIS_Line(m_originPoint, m_yEndPoint);
  m_zAxis = new AIS_Line(m_originPoint, m_zEndPoint);
}

void OccWorldAxes::configureAxes()
{
  if (!isValid()) return;

  constexpr Standard_Real width = 2.0;

  m_xAxis->SetColor(rgb(255, 0, 0));
  m_yAxis->SetColor(rgb(0, 180, 0));
  m_zAxis->SetColor(rgb(0, 0, 255));

  m_xAxis->SetWidth(width);
  m_yAxis->SetWidth(width);
  m_zAxis->SetWidth(width);

  m_xAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);
  m_yAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);
  m_zAxis->SetZLayer(Graphic3d_ZLayerId_Topmost);
}

Quantity_Color OccWorldAxes::rgb(const int r, const int g, const int b)
{
  return Quantity_Color(
    static_cast<Standard_Real>(r) / 255.0,
    static_cast<Standard_Real>(g) / 255.0,
    static_cast<Standard_Real>(b) / 255.0,
    Quantity_TOC_RGB
  );
}
