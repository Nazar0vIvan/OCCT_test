#pragma once

#include "3d/mathtypes.h"
#include "3d/occpartprops.h"

#include "cachedshapeloader.h"
#include "occpart.h"
#include "occviewcube.h"
#include "occworldaxes.h"

#include <Standard_Handle.hxx>

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
  using PartId = std::size_t;

  OccScene(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view, const QString& cadDir);
  ~OccScene() = default;

  OccScene(const OccScene&) = delete;
  OccScene& operator=(const OccScene&) = delete;

  OccScene(OccScene&&) noexcept = delete;
  OccScene& operator=(OccScene&&) noexcept = delete;

  [[nodiscard]] bool isValid() const;

  [[nodiscard]] std::optional<PartId> addStepPartWithId(const QString& stpFileName, const OccPartProps& props = {});
  [[nodiscard]] std::optional<PartId> addShapePartWithId(const TopoDS_Shape& shape, const OccPartProps& props = {});
  [[nodiscard]] bool setPartTransform(PartId id, const M4d& transform);

  void updateViewer();
  void updateCameraDependentObjects();
  void displayInfrastructure();

private:
  void displayWorldAxes();
  void redisplayWorldAxes();
  void displayViewCube();
  bool displayPart(OccPart& part);
  void activateAllSelectionModes(const OccPart& part);
  [[nodiscard]] double currentWorldAxisLength() const;

private:
  Handle(AIS_InteractiveContext) m_context;
  Handle(V3d_View) m_view;
  std::vector<OccPart> m_parts;
  CachedShapeLoader m_shapeLoader;
  OccWorldAxes m_worldAxes;
  OccViewCube m_viewCube;
  bool m_worldAxesDisplayed = false;
  bool m_viewCubeDisplayed = false;
};
