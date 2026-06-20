#include <QtTest>
#include <QStandardPaths>
#include "settings.h"

class TestSettings : public QObject {
    Q_OBJECT
    static void wipeConfig() {
        auto cfg = KSharedConfig::openConfig(QStringLiteral("wallpaperchangerrc"));
        cfg->deleteGroup(QStringLiteral("General"));
        cfg->sync();
    }
private slots:
    void initTestCase() { QStandardPaths::setTestModeEnabled(true); wipeConfig(); }
    void cleanupTestCase() { wipeConfig(); }
    void hasSpecDefaults() {
        Settings s;
        QVERIFY(s.folderPath().endsWith("/Pictures/Wallpapers"));
        QCOMPARE(s.hotkey(), QStringLiteral("Meta+O"));
        QCOMPARE(s.wallhavenCategories(), QStringLiteral("111"));
        QCOMPARE(s.wallhavenSort(), QStringLiteral("relevance"));
        QCOMPARE(s.fitMode(), QStringLiteral("Fill"));
    }
    void persistsChanges() {
        { Settings s; s.setFitMode("Tile"); s.setHotkey("Meta+P"); s.save(); }
        Settings s2;
        QCOMPARE(s2.fitMode(), QStringLiteral("Tile"));
        QCOMPARE(s2.hotkey(), QStringLiteral("Meta+P"));
    }
};

QTEST_GUILESS_MAIN(TestSettings)
#include "test_settings.moc"
