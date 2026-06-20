#pragma once
#include <QStringList>

class LocalLibrary {
public:
    static QStringList scanFolder(const QString &folder);
    static QStringList supportedExtensions();
};
