#include "wallhavenclient.h"
#include "wallhavenquery.h"
#include "wallhavenparser.h"
#include <utility>

WallhavenClient::WallhavenClient(QObject *parent, QString endpoint)
    : QObject(parent), m_endpoint(std::move(endpoint)) {
    m_debounce.setSingleShot(true);
    m_debounce.setInterval(350);   // 45 req/min budget -> debounce keystrokes
    connect(&m_debounce, &QTimer::timeout, this, [this]{ issue(1, false); });
}

void WallhavenClient::configure(const QString &categories, const QString &purity,
                                const QString &atleast, const QString &ratios,
                                const QString &sorting) {
    m_categories = categories; m_purity = purity;
    m_atleast = atleast; m_ratios = ratios; m_sorting = sorting;
}

void WallhavenClient::search(const QString &term) {
    m_term = term;
    m_currentPage = 1;
    m_debounce.start();            // restart debounce on each keystroke
}

void WallhavenClient::fetchNextPage() {
    if (m_currentPage >= m_lastPage) return;
    if (m_inflight) return;        // don't stack pages
    issue(m_currentPage + 1, true);
}

void WallhavenClient::cancel() {
    m_debounce.stop();
    if (m_inflight) { m_inflight->abort(); m_inflight.clear(); }
}

void WallhavenClient::issue(int page, bool append) {
    if (m_inflight) { m_inflight->abort(); m_inflight.clear(); }  // cancel in-flight
    const QUrl url = WallhavenQuery::build(m_term, m_categories, m_purity,
                                           m_atleast, m_ratios, m_sorting, page, m_endpoint);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "wallpaper-changer/0.1");
    QNetworkReply *reply = m_nam.get(req);
    m_inflight = reply;
    connect(reply, &QNetworkReply::finished, this, [this, reply, append]() {
        if (m_inflight == reply) m_inflight.clear();
        reply->deleteLater();
        if (reply->error() == QNetworkReply::OperationCanceledError) return;
        if (reply->error() != QNetworkReply::NoError) {
            const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            emit failed(code == 429 ? QStringLiteral("Wallhaven rate limit, slow down")
                                    : QStringLiteral("Couldn't reach Wallhaven"));
            return;
        }
        WallhavenMeta meta;
        const auto results = WallhavenParser::parse(reply->readAll(), &meta);
        m_currentPage = meta.currentPage > 0 ? meta.currentPage : m_currentPage;
        m_lastPage = meta.lastPage > 0 ? meta.lastPage : m_lastPage;
        emit resultsReady(results, meta, append);
    });
}
