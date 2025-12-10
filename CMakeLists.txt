cmake_minimum_required(VERSION 3.21)

project(Kreate LANGUAGES CXX)

# ---------------------------------------------------------
# Qt project settings
# ---------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---------------------------------------------------------
# Qt6 dependencies
# ---------------------------------------------------------
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Svg
)

# ---------------------------------------------------------
# KDE Frameworks (KF6)
# ---------------------------------------------------------
find_package(KF6Config REQUIRED)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6I18n REQUIRED)

# ---------------------------------------------------------
# C++ Sources
# ---------------------------------------------------------
qt_add_executable(kreate
    src/main.cpp
    src/core/ColorSchemeBuilder.cpp
    src/core/WallpaperAnalyzer.cpp
    src/io/ThemeWriter.cpp
    src/io/WallpaperLoader.cpp
    src/palette/PaletteGenerator.cpp
)

# ---------------------------------------------------------
# QML Module (Correct target: kreate)
# ---------------------------------------------------------
qt_add_qml_module(kreate
    URI Kreate
    VERSION 1.0

    QML_FILES
        src/ui/qml/main.qml
        src/ui/qml/OverviewPage.qml
        src/ui/qml/ThemePreview.qml
        src/ui/qml/ColorEditor.qml
        src/ui/qml/ExportPage.qml

    RESOURCES
        resources.qrc
)

target_include_directories(kreate PRIVATE
    src/core
    src/palette
    src/io
)

# ---------------------------------------------------------
# Link everything together
# ---------------------------------------------------------
target_link_libraries(kreate
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Svg

        KF6::ConfigCore
        KF6::CoreAddons
        KF6::I18n
)

# ---------------------------------------------------------
# Install
# ---------------------------------------------------------
install(TARGETS kreate)
