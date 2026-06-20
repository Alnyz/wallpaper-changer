#pragma once
#include <QString>
#include <QSize>

class ScreenInfo {
public:
    static QString atLeastFor(const QSize &px);
    static QString ratioFor(const QSize &px);
    static QSize physicalPrimaryResolution();
};
