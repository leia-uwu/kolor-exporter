project(kolor-exporter)
set(PROJECT_VERSION "1.0")
cmake_minimum_required(VERSION 3.16)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.102.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

include(ECMSetupVersion)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Config ConfigWidgets GuiAddons DBusAddons)

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
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::DBusAddons
    KF5::GuiAddons
)

install(TARGETS kolor-exporter DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kded)
