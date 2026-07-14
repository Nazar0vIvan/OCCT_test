#pragma once

#include <QWindow>

#include <QPoint>
#include <QPointF>

#include <memory>

#include "3d/mathtypes.h"

class QExposeEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;

class OccViewport;

class OccViewWindow final : public QWindow
{
  Q_OBJECT

public:
  explicit OccViewWindow(QWindow* parent = nullptr);
  ~OccViewWindow() override;

  OccViewWindow(const OccViewWindow&) = delete;
  OccViewWindow& operator=(const OccViewWindow&) = delete;

  OccViewWindow(OccViewWindow&&) noexcept = delete;
  OccViewWindow& operator=(OccViewWindow&&) noexcept = delete;

  void solveIK(const V6d& pose);
  void solveFK(const V6d& q);

protected:
  void exposeEvent(QExposeEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

private:
  void initializeViewportIfNeeded();
  [[nodiscard]] QPoint toNativePos(const QPointF& pos) const;
  void activateHost() const;

private:
  std::unique_ptr<OccViewport> m_viewport;
};
