#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include "locallibrary.h"

class TestLocalLibrary : public QObject {
    Q_OBJECT
    static void touch(const QString &path) {
        QFile f(path); f.open(QIODevice::WriteOnly); f.write("x"); f.close();
    }
private slots:
    void findsImagesRecursivelyAndSkipsNonImages() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        QDir(dir.path()).mkpath("sub");
        touch(dir.path() + "/a.jpg");
        touch(dir.path() + "/b.PNG");
        touch(dir.path() + "/sub/c.webp");
        touch(dir.path() + "/notes.txt");
        const QStringList found = LocalLibrary::scanFolder(dir.path());
        QCOMPARE(found.size(), 3);
        for (const QString &p : found) QVERIFY(QFileInfo(p).isAbsolute());
        QVERIFY(found.contains(dir.path() + "/a.jpg"));
        QVERIFY(found.contains(dir.path() + "/sub/c.webp"));
    }
    void emptyForMissingFolder() {
        QCOMPARE(LocalLibrary::scanFolder("/no/such/folder/xyz").size(), 0);
    }
};
QTEST_GUILESS_MAIN(TestLocalLibrary)
#include "test_locallibrary.moc"
