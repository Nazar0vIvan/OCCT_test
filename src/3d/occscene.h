#pragma once

#include "cachedshapeloader.h"
#include "occpart.h"
#include "occviewcube.h"
#include "occworldaxes.h"

#include <QString>

#include <Standard_Handle.hxx>

#include <Quantity_Color.hxx>

#include <gp_Trsf.hxx>

#include <cstddef>
#include <optional>
#include <vector>

class AIS_InteractiveContext;
class V3d_View;
class TopoDS_Shape;

class OccScene final
{
public:
  struct PartId
  {
    std::size_t value = 0;
  };

public:
  OccScene(
      const Handle(AIS_InteractiveContext)& context,
      const Handle(V3d_View)& view,
      const QString& cadDirectory
      );

  ~OccScene() = default;

  OccScene(const OccScene&) = delete;
  OccScene& operator=(const OccScene&) = delete;

  OccScene(OccScene&&) noexcept = delete;
  OccScene& operator=(OccScene&&) noexcept = delete;

  [[nodiscard]] bool isValid() const;

  bool loadStaticScene();

  bool addStepPart(
      const QString& stpFileName,
      const gp_Trsf& transform = gp_Trsf(),
      const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB),
      OccPart::SelectionMode selectionMode = OccPart::SelectionMode::None
      );

  bool addShapePart(
      const TopoDS_Shape& shape,
      const gp_Trsf& transform = gp_Trsf(),
      const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB),
      OccPart::SelectionMode selectionMode = OccPart::SelectionMode::None
      );

  [[nodiscard]] std::optional<PartId> addStepPartWithId(
      const QString& stpFileName,
      const gp_Trsf& transform = gp_Trsf(),
      const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB),
      OccPart::SelectionMode selectionMode = OccPart::SelectionMode::None,
      bool showTrihedron = false,
      double trihedronSize = 60.0
      );

  [[nodiscard]] std::optional<PartId> addShapePartWithId(
      const TopoDS_Shape& shape,
      const gp_Trsf& transform = gp_Trsf(),
      const Quantity_Color& color = Quantity_Color(0.72, 0.76, 0.80, Quantity_TOC_RGB),
      OccPart::SelectionMode selectionMode = OccPart::SelectionMode::None,
      bool showTrihedron = false,
      double trihedronSize = 60.0
      );

  [[nodiscard]] bool setPartTransform(PartId id, const gp_Trsf& transform);

  void updateViewer();
  void updateCameraDependentObjects();

private:
  void displayInfrastructure();
  void displayWorldAxes();
  void redisplayWorldAxes();

  void displayViewCube();

  bool displayPart(OccPart& part);
  void activateAllSelectionModes(const OccPart& part);

  [[nodiscard]] double currentWorldAxisLength() const;

  [[nodiscard]] static Quantity_Color rgb(int r, int g, int b);

private:
  Handle(AIS_InteractiveContext) m_context;
  Handle(V3d_View) m_view;

  CachedShapeLoader m_shapeLoader;

  // Append-only. Do not erase/reorder elements if external PartId values exist.
  std::vector<OccPart> m_parts;

  OccWorldAxes m_worldAxes;
  OccViewCube m_viewCube;

  bool m_worldAxesDisplayed = false;
  bool m_viewCubeDisplayed = false;
};
