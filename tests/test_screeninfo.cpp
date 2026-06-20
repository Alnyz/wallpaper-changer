#include <QtTest>
#include "screeninfo.h"

class TestScreenInfo : public QObject {
    Q_OBJECT
private slots:
    void atLeastFormatsSize() {
        QCOMPARE(ScreenInfo::atLeastFor(QSize(2560, 1440)), QStringLiteral("2560x1440"));
    }
    void ratioReducesByGcd() {
        QCOMPARE(ScreenInfo::ratioFor(QSize(2560, 1440)), QStringLiteral("16x9"));
        QCOMPARE(ScreenInfo::ratioFor(QSize(1920, 1080)), QStringLiteral("16x9"));
    }
};

QTEST_GUILESS_MAIN(TestScreenInfo)
#include "test_screeninfo.moc"
