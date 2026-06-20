#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QTimer>
#include "librarymodel.h"
#include "settings.h"
#include "wallpapersetter.h"
#include "wallhavenclient.h"

class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(LibraryModel *model READ model CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
public:
    explicit AppController(QObject *parent = nullptr);
    LibraryModel *model() { return &m_model; }
    QString status() const { return m_status; }

    Q_INVOKABLE void showLocal();
    Q_INVOKABLE void setQuery(const QString &term);
    Q_INVOKABLE void activate(int row);
    Q_INVOKABLE void loadMore();

    Q_INVOKABLE QString folderPath() const { return m_settings.folderPath(); }
    Q_INVOKABLE QString hotkey() const { return m_settings.hotkey(); }
    Q_INVOKABLE QString wallhavenCategories() const { return m_settings.wallhavenCategories(); }
    Q_INVOKABLE QString wallhavenSort() const { return m_settings.wallhavenSort(); }
    Q_INVOKABLE QString fitMode() const { return m_settings.fitMode(); }
    Q_INVOKABLE void saveSettings(const QString &folder, const QString &hotkey,
                                  const QString &categories, const QString &sort,
                                  const QString &fit);

signals:
    void statusChanged();

private:
    void setStatus(const QString &s);
    LibraryModel m_model;
    Settings m_settings;
    WallpaperSetter m_setter;
    WallhavenClient m_client;
    QNetworkAccessManager m_nam;
    QPointer<QNetworkReply> m_dl;
    QTimer m_dlTimeout;
    bool m_showingRemote = false;
    QString m_status;
};
