#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <KDBusService>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include "appcontroller.h"
#include "blurbridge.h"

static void configureLayerShell(QQuickWindow *win) {
    if (auto *ls = LayerShellQt::Window::get(win)) {
        ls->setLayer(LayerShellQt::Window::LayerOverlay);
        ls->setAnchors(LayerShellQt::Window::Anchors(
            LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorBottom |
            LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight));
        ls->setExclusiveZone(0);
        ls->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
    }
}

int main(int argc, char **argv) {
    LayerShellQt::Shell::useLayerShell();
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("WallpaperChanger"));
    app.setOrganizationDomain(QStringLiteral("alnyz.github.io"));
    app.setDesktopFileName(QStringLiteral("io.github.alnyz.WallpaperChanger"));

    KDBusService service(KDBusService::Unique);

    QQmlApplicationEngine engine;
    AppController controller;
    engine.rootContext()->setContextProperty(QStringLiteral("app"), &controller);
    BlurBridge blur;
    engine.rootContext()->setContextProperty(QStringLiteral("blur"), &blur);
    engine.loadFromModule("io.github.alnyz.WallpaperChanger", "Main");
    if (engine.rootObjects().isEmpty()) return 1;
    auto *win = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    if (!win) return 1;

    configureLayerShell(win);
    blur.setWindow(win);

    QObject::connect(win, SIGNAL(dismissRequested()), win, SLOT(hide()));

    auto showOverlay = [win]() {
        win->show();
        if (auto *ls = LayerShellQt::Window::get(win))
            ls->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
        win->requestActivate();
        win->raise();
    };

    QObject::connect(&service, &KDBusService::activateRequested, win,
        [showOverlay, &controller](const QStringList &, const QString &) {
            controller.showLocal();
            showOverlay();
        });

    controller.showLocal();
    showOverlay();
    return app.exec();
}
