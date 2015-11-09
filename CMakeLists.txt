project(muon)
set(PROJECT_VERSION "5.4.90")
set(PROJECT_VERSION_MAJOR 5)

cmake_minimum_required(VERSION 2.8.12)

option(MUON_INSTALL_EXPORTER "Install muon-exporter [development utility]" OFF)

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 5.2.0 REQUIRED CONFIG COMPONENTS Widgets Test Network Xml Concurrent DBus Svg Qml QuickWidgets)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)

find_package(KF5 REQUIRED WidgetsAddons CoreAddons Solid Archive
                                            Config Wallet DBusAddons I18n ConfigWidgets Notifications Declarative
                                            KIO IconThemes ItemViews)
find_package(KF5TextWidgets REQUIRED)

find_package(QApt 3.0.0 REQUIRED)
find_package(DebconfKDE 1.0.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(MUON_VERSION ${PROJECT_VERSION})
configure_file(MuonVersion.h.in MuonVersion.h)

add_subdirectory(src)

set_package_properties(QApt PROPERTIES
    DESCRIPTION "Qt wrapper around the libapt-pkg library"
    PURPOSE "Used to support apt-based distribution systems"
    TYPE REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
