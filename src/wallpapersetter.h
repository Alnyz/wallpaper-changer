#pragma once
#include <QString>
#include <QStringList>

class WallpaperSetter {
public:
    explicit WallpaperSetter(QString command = QStringLiteral("plasma-apply-wallpaperimage"));
    static QString fillModeFor(const QString &fitMode);
    static QStringList buildArgs(const QString &absPath, const QString &fitMode);
    bool apply(const QString &absPath, const QString &fitMode, QString *err = nullptr);
private:
    QString m_command;
};
