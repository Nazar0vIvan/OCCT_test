#pragma once

#include "stepimporter.h"

#include <QByteArray>
#include <QString>

#include <TopoDS_Shape.hxx>

struct CachedShapeResult
{
  bool ok = false;
  TopoDS_Shape shape;
  QString error;

  static CachedShapeResult success(const TopoDS_Shape& loadedShape)
  {
    CachedShapeResult result;
    result.ok = true;
    result.shape = loadedShape;
    return result;
  }

  static CachedShapeResult failure(const QString& message)
  {
    CachedShapeResult result;
    result.ok = false;
    result.error = message;
    return result;
  }
};

class CachedShapeLoader final
{
public:
  explicit CachedShapeLoader(QString cadDirectory);

  CachedShapeResult loadStpWithCache(const QString& stpFileName) const;

private:
  QString stepPathFor(const QString& stpFileName) const;
  QString brepPathFor(const QString& stpFileName) const;

  bool loadBRep(
    const QString& filePath,
    TopoDS_Shape& shape
  ) const;

  bool saveBRep(
    const QString& filePath,
    const TopoDS_Shape& shape
  ) const;

  static QByteArray toNativePathBytes(const QString& filePath);

private:
  QString m_cadDirectory;
  StepImporter m_stepImporter;
};