#pragma once

#include <QObject>
#include <QImage>
#include <QUrl>

class WallpaperLoader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY imageLoaded)
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl NOTIFY imageLoaded)
    Q_PROPERTY(bool hasImage READ hasImage NOTIFY imageLoaded)

public:
    explicit WallpaperLoader(QObject *parent = nullptr);

    QImage image() const { return m_image; }
    QUrl sourceUrl() const { return m_url; }
    bool hasImage() const { return !m_image.isNull(); }

    Q_INVOKABLE void load(const QUrl &url);

signals:
    void imageLoaded();
    void loadFailed(const QString &reason);

private:
    QImage m_image;
    QUrl   m_url;
};
