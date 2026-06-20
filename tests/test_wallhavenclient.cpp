#include <QtTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include "wallhavenclient.h"

// Point the client at a local server instead of wallhaven.cc so we can check
// deterministically whether a request was issued, without any real network.
class TestWallhavenClient : public QObject {
    Q_OBJECT

    static QString endpointFor(const QTcpServer &server) {
        return QStringLiteral("http://127.0.0.1:%1/api/v1/search").arg(server.serverPort());
    }
    static void drain(QTcpServer &server) {
        QObject::connect(&server, &QTcpServer::newConnection, &server, [&server] {
            while (QTcpSocket *s = server.nextPendingConnection()) s->deleteLater();
        });
    }

private slots:
    // Control: a debounced search does reach the endpoint (gives the cancel test teeth).
    void searchIssuesRequest() {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        QSignalSpy conns(&server, &QTcpServer::newConnection);
        drain(server);
        WallhavenClient client(nullptr, endpointFor(server));

        client.search("nature");
        QTRY_VERIFY_WITH_TIMEOUT(conns.count() > 0, 2000);

        client.cancel();
    }

    void cancelStopsDebouncedSearch() {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        QSignalSpy conns(&server, &QTcpServer::newConnection);
        drain(server);
        WallhavenClient client(nullptr, endpointFor(server));

        client.search("nature");
        client.cancel();
        QTest::qWait(800);   // past the 350ms debounce window

        QCOMPARE(conns.count(), 0);
    }

    void cancelWhenIdleIsNoop() {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        QSignalSpy conns(&server, &QTcpServer::newConnection);
        drain(server);
        WallhavenClient client(nullptr, endpointFor(server));

        client.cancel();
        QTest::qWait(200);

        QCOMPARE(conns.count(), 0);
    }
};
QTEST_GUILESS_MAIN(TestWallhavenClient)
#include "test_wallhavenclient.moc"
