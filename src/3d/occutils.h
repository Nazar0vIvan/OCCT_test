#pragma once

#include <Quantity_Color.hxx>
#include <gp_Trsf.hxx>

#include <algorithm>

inline const Quantity_Color kDefaultPartColor{0.72, 0.76, 0.80, Quantity_TOC_RGB};

enum class OccSelectionMode { PartOnly, All, None };

struct OccPartOptions
{
  gp_Trsf transform;
  Quantity_Color color = kDefaultPartColor;
  OccSelectionMode selectionMode = OccSelectionMode::None;
  bool showTrihedron = false;
  double trihedronSize = 60.0;
};

[[nodiscard]] inline Quantity_Color rgb(int r, int g, int b)
{
  return Quantity_Color(
    static_cast<double>(std::clamp(r, 0, 255)) / 255.0,
    static_cast<double>(std::clamp(g, 0, 255)) / 255.0,
    static_cast<double>(std::clamp(b, 0, 255)) / 255.0,
    Quantity_TOC_RGB
  );
}