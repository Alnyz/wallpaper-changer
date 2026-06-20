cmake_minimum_required(VERSION 3.16)
project(WallpaperChanger VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network Test)
find_package(KF6 REQUIRED COMPONENTS Config CoreAddons WindowSystem KIO I18n DBusAddons)
find_package(LayerShellQt REQUIRED)
include(KDEInstallDirs)

# --- core logic library (pure, unit-tested) ---
add_library(wpc_core STATIC
  src/screeninfo.cpp
  src/wallhavenquery.cpp
  src/wallhavenparser.cpp
  src/wallhavenclient.cpp
  src/locallibrary.cpp
  src/wallpapersetter.cpp
  src/downloader.cpp
  src/settings.cpp
  src/librarymodel.cpp
)
target_include_directories(wpc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(wpc_core PUBLIC Qt6::Core Qt6::Gui Qt6::Network KF6::ConfigCore)

enable_testing()
add_subdirectory(tests)

qt_add_executable(wallpaper-changer src/main.cpp src/appcontroller.cpp src/blurbridge.cpp)
qt_add_qml_module(wallpaper-changer
  URI io.github.alnyz.WallpaperChanger
  VERSION 1.0
  # Embed the module under the standard /qt/qml prefix so it sits on Qt's default
  # QML import path. Without this it lands at qrc:/io/... (off the default path),
  # which only resolves in the dev build (where the on-disk module dir is added to
  # the import path) and fails for the installed binary under D-Bus activation.
  RESOURCE_PREFIX "/qt/qml"
  QML_FILES
    qml/Main.qml
    qml/SearchBar.qml
    qml/Strip.qml
    qml/Tile.qml
    qml/SettingsPanel.qml
    qml/HelpPanel.qml
  RESOURCES
    qml/icons/configure.svg
    qml/icons/search.svg
    qml/icons/edit-clear.svg
    qml/icons/help.svg
)
target_link_libraries(wallpaper-changer PRIVATE
  wpc_core
  Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network
  KF6::WindowSystem KF6::KIOGui KF6::CoreAddons KF6::I18n KF6::DBusAddons
  LayerShellQt::Interface
)
install(TARGETS wallpaper-changer DESTINATION ${CMAKE_INSTALL_BINDIR})
install(PROGRAMS data/io.github.alnyz.WallpaperChanger.desktop
        DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES data/io.github.alnyz.WallpaperChanger.service
        DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
install(FILES data/io.github.alnyz.WallpaperChanger.svg
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps)
