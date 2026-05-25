#pragma once

#include <QObject>
#include <QWindow>
#include <QUrl>
#include <memory>

#include "occviewwindow.h"

class OccController : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QWindow* viewWindow READ viewWindow CONSTANT)

public:
  explicit OccController(QObject* parent = nullptr);
  ~OccController() override;

  QWindow* viewWindow() const;

  Q_INVOKABLE bool importStepFile(const QUrl& fileUrl);

private:
  std::unique_ptr<OccViewWindow> m_viewWindow;
};
