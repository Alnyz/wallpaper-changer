#include "settings.h"
#include <QStandardPaths>
#include <QDir>

Settings::Settings(const QString &configName)
    : m_config(KSharedConfig::openConfig(configName)),
      m_group(m_config, QStringLiteral("General")) {}

static QString defaultFolder() {
    const QString pics = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    return (pics.isEmpty() ? QDir::homePath() + "/Pictures" : pics) + "/Wallpapers";
}

QString Settings::folderPath() const { return m_group.readEntry("FolderPath", defaultFolder()); }
void Settings::setFolderPath(const QString &v) { m_group.writeEntry("FolderPath", v); }
QString Settings::hotkey() const { return m_group.readEntry("Hotkey", QStringLiteral("Meta+O")); }
void Settings::setHotkey(const QString &v) { m_group.writeEntry("Hotkey", v); }
QString Settings::wallhavenCategories() const { return m_group.readEntry("WallhavenCategories", QStringLiteral("111")); }
void Settings::setWallhavenCategories(const QString &v) { m_group.writeEntry("WallhavenCategories", v); }
QString Settings::wallhavenSort() const { return m_group.readEntry("WallhavenSort", QStringLiteral("relevance")); }
void Settings::setWallhavenSort(const QString &v) { m_group.writeEntry("WallhavenSort", v); }
QString Settings::fitMode() const { return m_group.readEntry("FitMode", QStringLiteral("Fill")); }
void Settings::setFitMode(const QString &v) { m_group.writeEntry("FitMode", v); }
void Settings::save() { m_group.sync(); }
