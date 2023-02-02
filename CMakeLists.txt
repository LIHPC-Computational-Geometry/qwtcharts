cmake_minimum_required(VERSION 3.12)

project (QwtCharts CXX)

include (GNUInstallDirs)
include (cmake/version.cmake)
include (cmake/common.cmake)


set (QwtCharts_VERSION ${QWT_CHARTS_VERSION})

set (CMAKE_VERBOSE_MAKEFILE ON)
option (BUILD_SHARED_LIBS "Creation de bibliotheques dynamiques (defaut : ON)" ON)

add_subdirectory (src)
