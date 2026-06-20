#pragma once
#include <QString>
#include <KSharedConfig>
#include <KConfigGroup>

class Settings {
public:
    explicit Settings(const QString &configName = QStringLiteral("wallpaperchangerrc"));
    QString folderPath() const;            void setFolderPath(const QString &);
    QString hotkey() const;                void setHotkey(const QString &);
    QString wallhavenCategories() const;   void setWallhavenCategories(const QString &);
    QString wallhavenSort() const;         void setWallhavenSort(const QString &);
    QString fitMode() const;               void setFitMode(const QString &);
    void save();
private:
    KSharedConfig::Ptr m_config;
    KConfigGroup m_group;
};
