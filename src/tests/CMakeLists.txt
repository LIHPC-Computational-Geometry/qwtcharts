find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (Qt5Widgets 5 REQUIRED NO_CMAKE_SYSTEM_PATH)

add_executable (qwt_test QtCurvesMainWindow.cpp curves_app.cpp public/QtCurvesMainWindow.h)	# *.h for automoc ...
set_property (TARGET qwt_test PROPERTY AUTOMOC ON)

target_include_directories (qwt_test PRIVATE ../QwtCharts/public ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_compile_options (qwt_test PRIVATE ${SHARED_CFLAGS}) # Requested by Qt ...
target_link_libraries (qwt_test PUBLIC QwtCharts)

# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
set_target_properties (qwt_test PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})

