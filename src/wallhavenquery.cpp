#include "wallhavenquery.h"
#include <QUrlQuery>

QUrl WallhavenQuery::build(const QString &term, const QString &categories, const QString &purity,
                          const QString &atleast, const QString &ratios,
                          const QString &sorting, int page, const QString &base) {
    QUrl url(base);
    QUrlQuery q;
    if (!term.isEmpty())    q.addQueryItem(QStringLiteral("q"), term);
    q.addQueryItem(QStringLiteral("categories"), categories);
    q.addQueryItem(QStringLiteral("purity"), purity);
    if (!atleast.isEmpty()) q.addQueryItem(QStringLiteral("atleast"), atleast);
    if (!ratios.isEmpty())  q.addQueryItem(QStringLiteral("ratios"), ratios);
    q.addQueryItem(QStringLiteral("sorting"), sorting);
    q.addQueryItem(QStringLiteral("order"), QStringLiteral("desc"));
    q.addQueryItem(QStringLiteral("page"), QString::number(page));
    url.setQuery(q);
    return url;
}
