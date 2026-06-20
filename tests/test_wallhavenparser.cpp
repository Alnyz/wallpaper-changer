#include <QtTest>
#include <QFile>
#include "wallhavenparser.h"

class TestWallhavenParser : public QObject {
    Q_OBJECT
    QByteArray fixture() {
        QFile f(QFINDTESTDATA("fixtures/wallhaven_search.json"));
        f.open(QIODevice::ReadOnly);
        return f.readAll();
    }
private slots:
    void parsesResults() {
        WallhavenMeta meta;
        const auto results = WallhavenParser::parse(fixture(), &meta);
        QCOMPARE(results.size(), 2);
        QCOMPARE(results[0].id, QStringLiteral("8grv2y"));
        QCOMPARE(results[0].path, QStringLiteral("https://w.wallhaven.cc/full/8g/wallhaven-8grv2y.png"));
        QCOMPARE(results[0].thumbSmall, QStringLiteral("https://th.wallhaven.cc/small/8g/8grv2y.jpg"));
        QCOMPARE(results[0].fileType, QStringLiteral("image/png"));
        QCOMPARE(meta.currentPage, 1);
        QCOMPARE(meta.lastPage, 42);
        QCOMPARE(meta.total, 1001);
    }
    void handlesGarbage() {
        WallhavenMeta meta;
        QCOMPARE(WallhavenParser::parse("not json", &meta).size(), 0);
    }
};
QTEST_GUILESS_MAIN(TestWallhavenParser)
#include "test_wallhavenparser.moc"
