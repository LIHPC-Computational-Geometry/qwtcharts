set (CURRENT_PACKAGE_NAME "QwtCharts")
set (CURRENT_PACKAGE_MACRO_NAME "QWT_CHARTS")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/FindQwt.cmake)	# définit ${QWT_TARGET}
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (QtUtil 6 REQUIRED)
find_package(Qt5Core 5 NO_CMAKE_SYSTEM_PATH)	# In order to enable moc ...

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp *.qrc)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (QwtCharts ${CPP_SOURCES} ${HEADERS})
set_property (TARGET QwtCharts PROPERTY AUTOMOC ON)
set_property (TARGET QwtCharts PROPERTY AUTORCC ON)	# => QwtCharts.qrc taken into account
set (ALL_TARGETS QwtCharts)
set_property (TARGET QwtCharts PROPERTY VERSION ${QWT_CHARTS_VERSION})
set_property (TARGET QwtCharts PROPERTY SOVERSION ${QWT_CHARTS_MAJOR_VERSION})
set (QWT_CHARTS_PUBLIC_FLAGS -DQT_5)
set (QWT_CHARTS_PRIVATE_FLAGS -DQWT_CHARTS_VERSION="${QWT_CHARTS_VERSION}")

target_include_directories (QwtCharts PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions (QwtCharts PUBLIC ${QWT_CHARTS_PUBLIC_FLAGS})
target_compile_definitions (QwtCharts PRIVATE ${QWT_CHARTS_PRIVATE_FLAGS})
target_compile_options (QwtCharts PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_include_directories (QwtCharts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_link_libraries (QwtCharts PUBLIC QtUtil::QtUtil ${QWT_TARGET})
# Etre capable une fois installée de retrouver TkUtil, Qt5*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libQwtCharts.so.5.0.0" to "") ...
set_target_properties (QwtCharts PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# Inefficace ici : CMAKE_AUTOMOC => dans le CMakeLists.txt racine
#set (CMAKE_AUTOMOC ON)
#set (CMAKE_AUTORCC ON)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/QwtChartsDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/QwtChartsDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/QwtChartsDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/QwtChartsDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})
# Dont QwtChartsDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QwtChartsDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QWT_CHARTS_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de QwtCharts :
set (INCLUDE_INSTALL_DIR include/)		# QwtCharts_INCLUDE_DIR avec QwtChartsConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# QwtCharts_LIB_DIR avec QwtChartsConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)			# QwtCharts_RUNTIME_DIR avec QwtChartsConfig.cmake.in
# Création du fichier QwtChartsConfig.cmake qui définira QwtCharts_INCLUDE_DIR, QwtCharts_LIB_DIR et QwtCharts_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier QwtChartsTargets.cmake dans prefix/lib/QwtCharts/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation})
# Copie des fichiers QwtChartsConfig.cmake et QwtChartsConfigVersion.cmake dans prefix/lib/QwtCharts/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)

