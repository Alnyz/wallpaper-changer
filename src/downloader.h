#pragma once
#include <QString>

class Downloader {
public:
    static QString targetPath(const QString &wallpapersFolder, const QString &id,
                              const QString &fileType);
};
