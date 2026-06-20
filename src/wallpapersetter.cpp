#include "wallpapersetter.h"
#include <QProcess>
#include <utility>

WallpaperSetter::WallpaperSetter(QString command) : m_command(std::move(command)) {}

QString WallpaperSetter::fillModeFor(const QString &fitMode) {
    if (fitMode == QLatin1String("Fit"))    return QStringLiteral("preserveAspect");
    if (fitMode == QLatin1String("Center")) return QStringLiteral("center");
    if (fitMode == QLatin1String("Tile"))   return QStringLiteral("tile");
    return QStringLiteral("preserveAspectCrop"); // Fill / default
}

QStringList WallpaperSetter::buildArgs(const QString &absPath, const QString &fitMode) {
    return { QStringLiteral("-f"), fillModeFor(fitMode), absPath };
}

bool WallpaperSetter::apply(const QString &absPath, const QString &fitMode, QString *err) {
    QProcess p;
    p.start(m_command, buildArgs(absPath, fitMode));
    if (!p.waitForStarted(3000)) {
        if (err) *err = QStringLiteral("could not start %1").arg(m_command);
        return false;
    }
    p.waitForFinished(10000);
    if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
        if (err) *err = QString::fromUtf8(p.readAllStandardError()).trimmed();
        return false;
    }
    return true;
}
