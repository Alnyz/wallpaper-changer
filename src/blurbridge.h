#pragma once
#include <QObject>
#include <QString>
class QWindow;

class BlurBridge : public QObject {
    Q_OBJECT
public:
    explicit BlurBridge(QObject *parent = nullptr) : QObject(parent) {}
    void setWindow(QWindow *w) { m_window = w; }
    Q_INVOKABLE void setBarRect(qreal x, qreal y, qreal w, qreal h);
    // Native KDE folder picker (kdialog). Hides the overlay while it's up, emits
    // folderChosen(path) on OK.
    Q_INVOKABLE void browseFolder(const QString &startDir);

signals:
    void folderChosen(const QString &path);

private:
    QWindow *m_window = nullptr;
};
