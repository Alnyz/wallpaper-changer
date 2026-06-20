#include "blurbridge.h"
#include <QWindow>
#include <QRegion>
#include <QRect>
#include <QProcess>
#include <QProcessEnvironment>
#include <memory>
#include <KWindowEffects>
#include <LayerShellQt/Window>

void BlurBridge::setBarRect(qreal x, qreal y, qreal w, qreal h) {
    if (!m_window) return;
    if (w <= 0 || h <= 0) return;
    const QRect r(qRound(x), qRound(y), qRound(w), qRound(h));
    KWindowEffects::enableBlurBehind(m_window, true, QRegion(r));
}

void BlurBridge::browseFolder(const QString &startDir) {
    if (!m_window) return;
    auto *proc = new QProcess(this);

    // Hide the overlay entirely so nothing composites behind the dialog.
    m_window->hide();

    // Guard so the two completion signals (finished / errorOccurred) only run once.
    auto guard = std::make_shared<bool>(false);
    auto complete = [this, proc, guard](bool ok) {
        if (*guard) return;
        *guard = true;

        // Re-summon the overlay (mirrors main.cpp's showOverlay). The open Settings
        // popup survives the hide/show, so it reappears in place.
        if (m_window) {
            m_window->show();
            if (auto *ls = LayerShellQt::Window::get(m_window))
                ls->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
            m_window->requestActivate();
            m_window->raise();
        }

        if (ok) {
            const QString path =
                QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
            if (!path.isEmpty()) emit folderChosen(path);
        }
        proc->deleteLater();
    };

    connect(proc, &QProcess::finished, this,
            [complete](int exitCode, QProcess::ExitStatus) { complete(exitCode == 0); });
    connect(proc, &QProcess::errorOccurred, this,
            [complete](QProcess::ProcessError) { complete(false); });

    // LayerShellQt::useLayerShell() exported QT_WAYLAND_SHELL_INTEGRATION=layer-shell
    // into our environment; a child Qt app (kdialog) would inherit it and try to
    // render itself as a layer-shell surface (flicker + "already has a shell
    // integration" warnings). Strip it so kdialog uses the normal xdg-shell.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("QT_WAYLAND_SHELL_INTEGRATION"));
    proc->setProcessEnvironment(env);

    QStringList args{ QStringLiteral("--title"), QStringLiteral("Choose wallpapers folder"),
                      QStringLiteral("--getexistingdirectory") };
    if (!startDir.isEmpty()) args << startDir;
    proc->start(QStringLiteral("kdialog"), args);
}
