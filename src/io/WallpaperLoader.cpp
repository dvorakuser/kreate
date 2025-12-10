#include "WallpaperLoader.h"

#include <QFileInfo>

WallpaperLoader::WallpaperLoader(QObject *parent)
    : QObject(parent)
{
}

void WallpaperLoader::load(const QUrl &url)
{
    m_url = url;
    const QString localPath = url.toLocalFile();

    if (localPath.isEmpty()) {
        emit loadFailed(QStringLiteral("Invalid URL"));
        return;
    }

    QImage img(localPath);
    if (img.isNull()) {
        emit loadFailed(QStringLiteral("Failed to load image: %1").arg(localPath));
        return;
    }

    m_image = img;
    emit imageLoaded();
}
