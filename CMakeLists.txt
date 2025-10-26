cmake_minimum_required(VERSION 3.27)
project(kreate LANGUAGES CXX)

# -----------------------------------------------------------------------------
#  General build settings (per KF6 guidelines)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# -----------------------------------------------------------------------------
#  Locate Qt6 and KF6 (auto-detect + fallback prefix path)
# -----------------------------------------------------------------------------
# You can override this manually if needed:
# cmake -B build -DCMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake/Qt6:/usr/lib/x86_64-linux-gnu/cmake/KF6"

if(NOT DEFINED CMAKE_PREFIX_PATH)
    # Try system-wide installs (default on Kubuntu / Neon)
    set(CMAKE_PREFIX_PATH
        "/usr/lib/x86_64-linux-gnu/cmake/Qt6"
        "/usr/lib/x86_64-linux-gnu/cmake/KF6"
        CACHE STRING "Search path for Qt6 and KF6 frameworks" FORCE
    )
endif()

# -----------------------------------------------------------------------------
#  Find ECM (Extra CMake Modules) first
# -----------------------------------------------------------------------------
find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

# -----------------------------------------------------------------------------
#  Find Qt6 and KF6 Frameworks (per docs)
# -----------------------------------------------------------------------------
find_package(Qt6 6.6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

find_package(KF6 6.0 REQUIRED COMPONENTS
    Kirigami
    I18n
    CoreAddons
    Config
    Package
)

# -----------------------------------------------------------------------------
#  Project sources
# -----------------------------------------------------------------------------
set(SRC_FILES
    src/main.cpp
    src/backend/colorschemebuilder.cpp
    src/backend/colorschemebuilder.h
    src/backend/metadatawriter.cpp
    src/backend/validator.cpp
    src/backend/validator.h
    src/backend/packagewriter.cpp
    src/backend/packagewriter.h
    resources.qrc
    qml.qrc
)

# -----------------------------------------------------------------------------
#  Target definition
# -----------------------------------------------------------------------------
qt_add_executable(kreate
    MANUAL_FINALIZATION
    ${SRC_FILES}
)

# -----------------------------------------------------------------------------
#  Link required libraries
# -----------------------------------------------------------------------------
target_link_libraries(kreate PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    KF6::Kirigami
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::Package
)

# -----------------------------------------------------------------------------
#  Finalization per Qt6 build docs
# -----------------------------------------------------------------------------
qt_import_qml_plugins(kreate)
qt_finalize_executable(kreate)

# -----------------------------------------------------------------------------
#  Install rules (KF6 layout)
# -----------------------------------------------------------------------------
install(TARGETS kreate
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)
