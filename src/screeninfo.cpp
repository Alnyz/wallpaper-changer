#include "screeninfo.h"
#include <QGuiApplication>
#include <QScreen>
#include <numeric>

QString ScreenInfo::atLeastFor(const QSize &px) {
    return QStringLiteral("%1x%2").arg(px.width()).arg(px.height());
}

QString ScreenInfo::ratioFor(const QSize &px) {
    const int g = std::gcd(px.width(), px.height());
    if (g <= 0) return QStringLiteral("16x9");
    return QStringLiteral("%1x%2").arg(px.width() / g).arg(px.height() / g);
}

QSize ScreenInfo::physicalPrimaryResolution() {
    if (QScreen *s = QGuiApplication::primaryScreen()) {
        return s->size() * s->devicePixelRatio();
    }
    return QSize(1920, 1080);
}
