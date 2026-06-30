#pragma once

#include "cachedshapeloader.h"
#include "occpart.h"
#include "occviewcube.h"
#include "occworldaxes.h"

#include <Standard_Handle.hxx>
#include <Quantity_Color.hxx>
#include <gp_Trsf.hxx>

#include <QString>

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
  OccScene(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view, const QString& cadDir);

  ~OccScene() = default;

  OccScene(const OccScene&) = delete;
  OccScene& operator=(const OccScene&) = delete;

  OccScene(OccScene&&) noexcept = delete;
  OccScene& operator=(OccScene&&) noexcept = delete;

  [[nodiscard]] bool isValid() const;

  bool loadStaticScene();

  [[nodiscard]] std::optional<PartId> addStepPartWithId(const QString& stpFileName, const OccPartOptions& options = {});
  [[nodiscard]] std::optional<PartId> addShapePartWithId(const TopoDS_Shape& shape, const OccPartOptions& options = {});
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

private:
  Handle(AIS_InteractiveContext) m_context;
  Handle(V3d_View) m_view;

  CachedShapeLoader m_shapeLoader;

  std::vector<OccPart> m_parts;

  OccWorldAxes m_worldAxes;
  OccViewCube m_viewCube;

  bool m_worldAxesDisplayed = false;
  bool m_viewCubeDisplayed = false;
};
