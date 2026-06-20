#include "downloader.h"

QString Downloader::targetPath(const QString &wallpapersFolder, const QString &id,
                               const QString &fileType) {
    QString ext = fileType;
    ext.remove(QStringLiteral("image/"));
    if (ext == QLatin1String("jpeg")) ext = QStringLiteral("jpg");
    if (ext.isEmpty()) ext = QStringLiteral("jpg");
    return QStringLiteral("%1/wallhaven/wallhaven-%2.%3").arg(wallpapersFolder, id, ext);
}
