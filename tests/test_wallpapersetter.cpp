#include <QtTest>
#include "wallpapersetter.h"

class TestWallpaperSetter : public QObject {
    Q_OBJECT
private slots:
    void mapsFitModes() {
        QCOMPARE(WallpaperSetter::fillModeFor("Fill"),   QStringLiteral("preserveAspectCrop"));
        QCOMPARE(WallpaperSetter::fillModeFor("Fit"),    QStringLiteral("preserveAspect"));
        QCOMPARE(WallpaperSetter::fillModeFor("Center"), QStringLiteral("center"));
        QCOMPARE(WallpaperSetter::fillModeFor("Tile"),   QStringLiteral("tile"));
        QCOMPARE(WallpaperSetter::fillModeFor("???"),    QStringLiteral("preserveAspectCrop"));
    }
    void buildsArgs() {
        const QStringList a = WallpaperSetter::buildArgs("/home/u/Pictures/x.png", "Fill");
        QCOMPARE(a, (QStringList{ "-f", "preserveAspectCrop", "/home/u/Pictures/x.png" }));
    }
};
QTEST_GUILESS_MAIN(TestWallpaperSetter)
#include "test_wallpapersetter.moc"
