#pragma once
#include <QObject>
#include <QString>
#include <QTimer>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "wallhavenresult.h"

class WallhavenClient : public QObject {
    Q_OBJECT
public:
    explicit WallhavenClient(QObject *parent = nullptr,
                             QString endpoint = QStringLiteral("https://wallhaven.cc/api/v1/search"));
    void configure(const QString &categories, const QString &purity,
                   const QString &atleast, const QString &ratios, const QString &sorting);
    void search(const QString &term);     // debounced; resets to page 1
    void fetchNextPage();                  // append next page of current term
    void cancel();                         // stop the debounce and abort any in-flight request

signals:
    void resultsReady(const QList<WallhavenResult> &results, const WallhavenMeta &meta, bool append);
    void failed(const QString &message);

private:
    void issue(int page, bool append);
    QNetworkAccessManager m_nam;
    QTimer m_debounce;
    QPointer<QNetworkReply> m_inflight;
    QString m_endpoint;
    QString m_term, m_categories{"111"}, m_purity{"100"}, m_atleast, m_ratios, m_sorting{"relevance"};
    int m_currentPage = 1;
    int m_lastPage = 1;
};
