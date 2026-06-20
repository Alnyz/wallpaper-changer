#include <QtTest>
#include <QUrlQuery>
#include "wallhavenquery.h"

class TestWallhavenQuery : public QObject {
    Q_OBJECT
private slots:
    void buildsSfwSearchUrl() {
        const QUrl u = WallhavenQuery::build("nature", "111", "100",
                                             "2560x1440", "16x9", "relevance", 1);
        QCOMPARE(u.scheme(), QStringLiteral("https"));
        QCOMPARE(u.host(), QStringLiteral("wallhaven.cc"));
        QCOMPARE(u.path(), QStringLiteral("/api/v1/search"));
        const QUrlQuery q(u);
        QCOMPARE(q.queryItemValue("q"), QStringLiteral("nature"));
        QCOMPARE(q.queryItemValue("categories"), QStringLiteral("111"));
        QCOMPARE(q.queryItemValue("purity"), QStringLiteral("100"));
        QCOMPARE(q.queryItemValue("atleast"), QStringLiteral("2560x1440"));
        QCOMPARE(q.queryItemValue("ratios"), QStringLiteral("16x9"));
        QCOMPARE(q.queryItemValue("sorting"), QStringLiteral("relevance"));
        QCOMPARE(q.queryItemValue("order"), QStringLiteral("desc"));
        QCOMPARE(q.queryItemValue("page"), QStringLiteral("1"));
    }
    void omitsEmptyOptionalParams() {
        const QUrl u = WallhavenQuery::build("", "111", "100", "", "", "relevance", 2);
        const QUrlQuery q(u);
        QVERIFY(!q.hasQueryItem("atleast"));
        QVERIFY(!q.hasQueryItem("ratios"));
        QCOMPARE(q.queryItemValue("page"), QStringLiteral("2"));
    }
};
QTEST_GUILESS_MAIN(TestWallhavenQuery)
#include "test_wallhavenquery.moc"
