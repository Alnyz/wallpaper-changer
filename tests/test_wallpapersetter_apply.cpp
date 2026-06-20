#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include "wallpapersetter.h"

class TestWallpaperSetterApply : public QObject {
    Q_OBJECT
private slots:
    void runsStubCommandWithArgs() {
        QTemporaryDir dir;
        const QString rec = dir.path() + "/argv.txt";
        const QString stub = dir.path() + "/fake-cmd";
        {
            QFile f(stub);
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write(("#!/bin/sh\nprintf '%s\\n' \"$@\" > '" + rec + "'\n").toUtf8());
            f.close();
            QFile::setPermissions(stub, QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                        QFileDevice::ExeOwner);
        }
        WallpaperSetter setter(stub);
        QString err;
        QVERIFY2(setter.apply("/tmp/pic.png", "Fill", &err), qPrintable(err));
        QFile out(rec); QVERIFY(out.open(QIODevice::ReadOnly));
        const QStringList argv = QString::fromUtf8(out.readAll()).split('\n', Qt::SkipEmptyParts);
        QCOMPARE(argv, (QStringList{ "-f", "preserveAspectCrop", "/tmp/pic.png" }));
    }
};
QTEST_GUILESS_MAIN(TestWallpaperSetterApply)
#include "test_wallpapersetter_apply.moc"
