#include "locallibrary.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

QStringList LocalLibrary::supportedExtensions() {
    return { "jpg", "jpeg", "png", "webp", "bmp", "gif" };
}

QStringList LocalLibrary::scanFolder(const QString &folder) {
    QStringList out;
    QDir d(folder);
    if (!d.exists()) return out;
    const QStringList exts = supportedExtensions();
    QDirIterator it(folder, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString path = it.next();
        if (exts.contains(QFileInfo(path).suffix().toLower()))
            out.append(path);
    }
    out.sort();
    return out;
}
