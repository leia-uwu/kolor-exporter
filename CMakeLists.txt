cmake_minimum_required(VERSION 3.16)

project(kolor-exporter)
set(PROJECT_VERSION "1.0")

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.0.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS ColorScheme CoreAddons Config GuiAddons DBusAddons WindowSystem)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_library(kolor-exporter MODULE)

target_sources(kolor-exporter
  PRIVATE
    kolorExporter.cpp
)

target_compile_definitions(kolor-exporter
  PUBLIC
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_link_libraries(kolor-exporter
  PUBLIC
    KF6::ColorScheme
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::DBusAddons
    KF6::GuiAddons
)

install(TARGETS kolor-exporter DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kded)
