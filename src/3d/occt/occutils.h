#pragma once

#include <Quantity_Color.hxx>

#include <algorithm>

[[nodiscard]] inline Quantity_Color rgb(int r, int g, int b)
{
  return Quantity_Color(
    static_cast<double>(std::clamp(r, 0, 255)) / 255.0,
    static_cast<double>(std::clamp(g, 0, 255)) / 255.0,
    static_cast<double>(std::clamp(b, 0, 255)) / 255.0,
    Quantity_TOC_RGB
  );
}
