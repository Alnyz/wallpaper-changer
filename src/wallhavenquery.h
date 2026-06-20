#pragma once
#include <QUrl>
#include <QString>

class WallhavenQuery {
public:
    static QUrl build(const QString &term, const QString &categories, const QString &purity,
                      const QString &atleast, const QString &ratios,
                      const QString &sorting, int page,
                      const QString &base = QStringLiteral("https://wallhaven.cc/api/v1/search"));
};
