project(muon)
set(PROJECT_VERSION "5.5.0")
set(PROJECT_VERSION_MAJOR 5)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 5.2.0 REQUIRED CONFIG COMPONENTS Widgets Concurrent)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)

find_package(KF5 REQUIRED KIO DBusAddons I18n IconThemes XmlGui)

find_package(QApt 3.0.0 REQUIRED)
find_package(DebconfKDE 1.0.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(MUON_VERSION ${PROJECT_VERSION})
configure_file(MuonVersion.h.in MuonVersion.h)

add_subdirectory(src)

set_package_properties(QApt PROPERTIES
    DESCRIPTION "Qt wrapper around the libapt-pkg library"
    PURPOSE "Used to support apt-based distribution systems"
    TYPE REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
