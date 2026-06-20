#include "appcontroller.h"
#include "locallibrary.h"
#include "downloader.h"
#include <QNetworkRequest>
#include <QFile>
#include <QFileInfo>
#include <QDir>

AppController::AppController(QObject *parent) : QObject(parent) {
    m_client.configure(m_settings.wallhavenCategories(), QStringLiteral("100"),
                       QStringLiteral("1920x1080"), QStringLiteral("16x9"),
                       m_settings.wallhavenSort());
    connect(&m_client, &WallhavenClient::resultsReady, this,
        [this](const QList<WallhavenResult> &r, const WallhavenMeta &, bool append) {
            if (!m_showingRemote) return;   // ignore late results once we've switched to local
            if (append) m_model.appendRemoteResults(r);
            else        m_model.setRemoteResults(r);
            setStatus(r.isEmpty() && !append ? QStringLiteral("No results") : QString());
        });
    connect(&m_client, &WallhavenClient::failed, this,
        [this](const QString &m) { if (m_showingRemote) setStatus(m); });

    m_dlTimeout.setSingleShot(true);
    connect(&m_dlTimeout, &QTimer::timeout, this, [this]() {
        if (m_dl) { m_dl->abort(); setStatus(QStringLiteral("Download timed out, try again")); }
    });
}

void AppController::setStatus(const QString &s) {
    if (s == m_status) return;
    m_status = s; emit statusChanged();
}

void AppController::showLocal() {
    // Switching to local: clear the remote flag and cancel any in-flight remote
    // work so a late search result or download can't overwrite the local view.
    m_showingRemote = false;
    m_client.cancel();
    if (m_dl) m_dl->abort();
    const QStringList paths = LocalLibrary::scanFolder(m_settings.folderPath());
    m_model.setLocalPaths(paths);
    setStatus(paths.isEmpty()
        ? QStringLiteral("No images in %1").arg(m_settings.folderPath())
        : QString());
}

void AppController::setQuery(const QString &term) {
    if (term.trimmed().isEmpty()) { showLocal(); return; }
    m_showingRemote = true;
    setStatus(QStringLiteral("Searching…"));
    m_client.search(term);
}

void AppController::activate(int row) {
    if (row < 0 || row >= m_model.rowCount()) return;
    if (m_dl) m_dl->abort();   // a new activation supersedes any in-flight download
    const LibraryModel::Item it = m_model.itemAt(row);
    if (it.isLocal) {
        QString err;
        if (m_setter.apply(it.sourcePath, m_settings.fitMode(), &err)) {
            m_model.setSelectedIndex(row);
            setStatus(QString());
        } else {
            setStatus(QStringLiteral("Failed to set wallpaper: %1").arg(err));
        }
        return;
    }

    const QString dest = Downloader::targetPath(m_settings.folderPath(), it.wid, it.fileType);
    QDir().mkpath(QFileInfo(dest).absolutePath());

    setStatus(QStringLiteral("Downloading…"));

    QNetworkRequest req{QUrl(it.sourcePath)};
    req.setHeader(QNetworkRequest::UserAgentHeader, QByteArrayLiteral("wallpaper-changer/0.1"));
    QNetworkReply *reply = m_nam.get(req);
    m_dl = reply;
    m_dlTimeout.start(25000);          // stall timeout; reset on each progress tick below
    const int targetRow = row;

    connect(reply, &QNetworkReply::downloadProgress, this, [this, reply](qint64 received, qint64 total) {
        if (m_dl != reply) return;
        m_dlTimeout.start(25000);      // got data -> not stalled
        if (total > 0)
            setStatus(QStringLiteral("Downloading… %1%").arg(received * 100 / total));
        else if (received > 0)
            setStatus(QStringLiteral("Downloading… %1 KB").arg(received / 1024));
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, dest, targetRow]() {
        const bool current = (m_dl == reply);
        if (current) { m_dl.clear(); m_dlTimeout.stop(); }
        reply->deleteLater();
        if (reply->error() == QNetworkReply::OperationCanceledError) return;
        if (reply->error() != QNetworkReply::NoError) {
            setStatus(QStringLiteral("Download failed: %1").arg(reply->errorString()));
            return;
        }
        QFile f(dest);
        if (!f.open(QIODevice::WriteOnly) || f.write(reply->readAll()) < 0) {
            setStatus(QStringLiteral("Download failed: cannot write %1").arg(dest));
            return;
        }
        f.close();
        QString err;
        if (m_setter.apply(dest, m_settings.fitMode(), &err)) {
            m_model.setSelectedIndex(targetRow);
            setStatus(QString());
        } else {
            setStatus(QStringLiteral("Failed to set wallpaper: %1").arg(err));
        }
    });
}

void AppController::loadMore() {
    if (m_showingRemote) m_client.fetchNextPage();
}

void AppController::saveSettings(const QString &folder, const QString &hotkey,
                                 const QString &categories, const QString &sort,
                                 const QString &fit) {
    m_settings.setFolderPath(folder);
    m_settings.setHotkey(hotkey);
    m_settings.setWallhavenCategories(categories);
    m_settings.setWallhavenSort(sort);
    m_settings.setFitMode(fit);
    m_settings.save();
    m_client.configure(categories, QStringLiteral("100"),
                       QStringLiteral("1920x1080"), QStringLiteral("16x9"), sort);
    if (!m_showingRemote) showLocal();
}
