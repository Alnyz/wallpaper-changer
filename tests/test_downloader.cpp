#include <QtTest>
#include "downloader.h"

class TestDownloader : public QObject {
    Q_OBJECT
private slots:
    void buildsTargetPathByMime() {
        QCOMPARE(Downloader::targetPath("/home/u/Pictures/Wallpapers", "8grv2y", "image/png"),
                 QStringLiteral("/home/u/Pictures/Wallpapers/wallhaven/wallhaven-8grv2y.png"));
        QCOMPARE(Downloader::targetPath("/home/u/Pictures/Wallpapers", "abc123", "image/jpeg"),
                 QStringLiteral("/home/u/Pictures/Wallpapers/wallhaven/wallhaven-abc123.jpg"));
    }
};
QTEST_GUILESS_MAIN(TestDownloader)
#include "test_downloader.moc"
