#include "occinputcontroller.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_SelectionScheme.hxx>
#include <AIS_ViewCube.hxx>

#include <Standard_TypeDef.hxx>

#include <V3d_View.hxx>

OccInputController::OccInputController(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view)
  : m_context(context),
    m_view(view)
{
}

bool OccInputController::isValid() const
{
  return !m_context.IsNull() && !m_view.IsNull();
}

OccInputResult OccInputController::mousePress(const QPoint& pos, const Qt::MouseButton button)
{
  if (!isValid()) return {};

  m_lastMousePos = pos;

  switch (button) {
    case Qt::LeftButton: return handleLeftButtonPress(pos);
    case Qt::RightButton: return handleRightButtonPress(pos);
    case Qt::MiddleButton: return handleMiddleButtonPress(pos);
    default:  return {};
  }
}

OccInputResult OccInputController::mouseMove(const QPoint& pos)
{
  if (!isValid()) return {};

  if (m_rotating) return handleRotationMove(pos);

  if (m_panning) return handlePanMove(pos);

  return handleHoverMove(pos);
}

OccInputResult OccInputController::mouseRelease(const Qt::MouseButton button)
{
  if (!isValid()) return {};

  if (button == Qt::RightButton) {
    m_rotating = false;

    OccInputResult result;
    result.accepted = true;
    return result;
  }

  if (button == Qt::MiddleButton) {
    m_panning = false;

    OccInputResult result;
    result.accepted = true;
    return result;
  }

  return {};
}

OccInputResult OccInputController::wheel(const QPoint& pos, const int angleDeltaY)
{
  if (!isValid()) return {};

  if (angleDeltaY == 0) return {};

  if (angleDeltaY > 0) {
    m_view->Zoom(
      pos.x(),
      pos.y(),
      pos.x() + 10,
      pos.y() + 10
    );
  } else {
    m_view->Zoom(
      pos.x(),
      pos.y(),
      pos.x() - 10,
      pos.y() - 10
    );
  }

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.needsViewerUpdate = true;
  result.cameraChanged = true;

  return result;
}

OccInputResult OccInputController::handleLeftButtonPress(const QPoint& pos)
{
  m_context->MoveTo(
    pos.x(),
    pos.y(),
    m_view,
    Standard_True
  );

  if (!m_context->HasDetected()) {
    return clearSelection();
  }

  Handle(AIS_ViewCubeOwner) cubeOwner = Handle(AIS_ViewCubeOwner)::DownCast(m_context->DetectedOwner());

  if (!cubeOwner.IsNull()) {
    return handleDetectedViewCubeOwner();
  }

  return handleDetectedSelectable();
}

OccInputResult OccInputController::handleRightButtonPress(const QPoint& pos)
{
  m_rotating = true;
  m_panning = false;

  m_view->StartRotation(pos.x(), pos.y());

  OccInputResult result;
  result.accepted = true;

  return result;
}

OccInputResult OccInputController::handleMiddleButtonPress(const QPoint& pos)
{
  Q_UNUSED(pos);

  m_panning = true;
  m_rotating = false;

  OccInputResult result;
  result.accepted = true;

  return result;
}

OccInputResult OccInputController::handleRotationMove(const QPoint& pos)
{
  m_view->Rotation(pos.x(), pos.y());

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.cameraChanged = true;

  return result;
}

OccInputResult OccInputController::handlePanMove(const QPoint& pos)
{
  const QPoint delta = pos - m_lastMousePos;

  m_view->Pan(delta.x(), -delta.y());

  m_lastMousePos = pos;

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.cameraChanged = true;

  return result;
}

OccInputResult OccInputController::handleHoverMove(const QPoint& pos)
{
  m_context->MoveTo(
    pos.x(),
    pos.y(),
    m_view,
    Standard_True
  );

  OccInputResult result;
  result.accepted = true;
  result.hoverChanged = true;

  return result;
}

OccInputResult OccInputController::handleDetectedViewCubeOwner()
{
  Handle(AIS_ViewCubeOwner) cubeOwner = Handle(AIS_ViewCubeOwner)::DownCast(m_context->DetectedOwner());

  if (cubeOwner.IsNull()) {
    return {};
  }

  m_view->SetProj(cubeOwner->MainOrientation());
  m_view->ZFitAll();

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.needsViewerUpdate = true;
  result.cameraChanged = true;

  return result;
}

OccInputResult OccInputController::handleDetectedSelectable()
{
  m_context->SelectDetected(AIS_SelectionScheme_Replace);

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.needsViewerUpdate = true;
  result.selectionChanged = true;

  return result;
}

OccInputResult OccInputController::clearSelection()
{
  m_context->ClearSelected(Standard_False);

  OccInputResult result;
  result.accepted = true;
  result.needsRedraw = true;
  result.needsViewerUpdate = true;
  result.selectionChanged = true;

  return result;
}
