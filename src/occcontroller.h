#pragma once

#include <QObject>
#include <QWindow>

#include <memory>

class OccViewWindow;

class OccController final : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QWindow* viewWindow READ viewWindow CONSTANT)

public:
  explicit OccController(QObject* parent = nullptr);
  ~OccController() override;

  Q_DISABLE_COPY_MOVE(OccController)

  QWindow* viewWindow() const;

private:
  std::unique_ptr<OccViewWindow> m_viewWindow;
};
