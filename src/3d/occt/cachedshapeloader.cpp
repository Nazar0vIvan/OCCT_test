#include "cachedshapeloader.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

CachedShapeLoader::CachedShapeLoader(QString cadDirectory) : m_cadDirectory(QDir::fromNativeSeparators(std::move(cadDirectory)))
{}

CachedShapeResult CachedShapeLoader::loadStpWithCache(const QString& stpFileName) const
{
  if (m_cadDirectory.trimmed().isEmpty()) {
    return CachedShapeResult::failure(
      QStringLiteral("Cached shape loading failed: CAD directory is empty")
    );
  }

  const QDir cadDir(m_cadDirectory);

  if (!cadDir.exists()) {
    return CachedShapeResult::failure(QStringLiteral("Cached shape loading failed: CAD directory does not exist: %1")
      .arg(m_cadDirectory)
    );
  }

  const QString stepPath = stepPathFor(stpFileName);
  const QString brepPath = brepPathFor(stpFileName);

  TopoDS_Shape cachedShape;

  if (QFileInfo::exists(brepPath)) {
    if (loadBRep(brepPath, cachedShape) && !cachedShape.IsNull()) {
      qDebug() << "Loaded BREP cache:" << brepPath;
      return CachedShapeResult::success(cachedShape);
    }

    qWarning() << "Failed to load BREP cache, falling back to STEP:" << brepPath;
  }

  if (!QFileInfo::exists(stepPath)) {
    return CachedShapeResult::failure(
      QStringLiteral("Cached shape loading failed: STEP file does not exist: %1").arg(stepPath)
    );
  }

  const CadImportResult stepResult = m_stepImporter.importFile(stepPath);

  if (!stepResult.ok) {
    return CachedShapeResult::failure(stepResult.error);
  }

  if (!saveBRep(brepPath, stepResult.shape)) {
    qWarning() << "Failed to save BREP cache:" << brepPath;
  } else {
    qDebug() << "Saved BREP cache:" << brepPath;
  }

  return CachedShapeResult::success(stepResult.shape);
}

QString CachedShapeLoader::stepPathFor(const QString& stpFileName) const
{
  const QDir cadDir(m_cadDirectory);
  return cadDir.filePath(stpFileName);
}

QString CachedShapeLoader::brepPathFor(const QString& stpFileName) const
{
  const QFileInfo stpInfo(stpFileName);

  const QString baseName = stpInfo.completeBaseName();

  const QDir cadDir(m_cadDirectory);
  return cadDir.filePath(baseName + QStringLiteral(".brep"));
}

bool CachedShapeLoader::loadBRep(const QString& filePath, TopoDS_Shape& shape) const
{
  BRep_Builder builder;

  return BRepTools::Read(shape, toNativePathBytes(filePath).constData(), builder);
}

bool CachedShapeLoader::saveBRep(const QString& filePath, const TopoDS_Shape& shape) const
{
  if (shape.IsNull()) return false;

  return BRepTools::Write(shape, toNativePathBytes(filePath).constData());
}

QByteArray CachedShapeLoader::toNativePathBytes(const QString& filePath)
{
  const QString nativePath = QDir::toNativeSeparators(filePath);

  #ifdef Q_OS_WIN
    return nativePath.toLocal8Bit();
  #else
    return nativePath.toUtf8();
  #endif
}
