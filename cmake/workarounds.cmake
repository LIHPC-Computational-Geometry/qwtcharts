#
# workarounds.cmake
#
# Version 0.9.0 (27/09/2021)
#
# On définit ici les contournement aux problèmes de type build-system rencontrés dans le projet
#
include (CMakeDetermineCXXCompiler)

message ("CMAKE_SYSTEM_NAME=" ${CMAKE_SYSTEM_NAME})
find_program(CCC_OS_FOUND ccc_os)
if (CCC_OS_FOUND)
	execute_process (COMMAND ccc_os OUTPUT_VARIABLE PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
endif (CCC_OS_FOUND)

if (NOT PLATFORM)
	execute_process (COMMAND lsb_release -d COMMAND awk "{print $2;}" OUTPUT_VARIABLE PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
endif (NOT PLATFORM)
message ("PLATFORM=" ${PLATFORM})
message ("LANGUAGES=" ${LANGUAGES})
message ("CXX=" ${CXX})
message ("CMAKE_CXX_COMPILER=" ${CMAKE_CXX_COMPILER})
message ("_CMAKE_TOOLCHAIN_PREFIX=" ${_CMAKE_TOOLCHAIN_PREFIX})
message ("COMPILER_BASENAME=" ${COMPILER_BASENAME})

# Les répertoires de recherche par le compilateur des bibliothèques (a priori portable g++/icpc/clang++) :
# Méthode 1 (ça marche), en passant par bash :
#execute_process(COMMAND bash "-c" "LANG=C ${CMAKE_CXX_COMPILER} -print-search-dirs  | sed -n 's/libraries: =//gp' | tr -s ':' ';'" OUTPUT_VARIABLE GNU_LIBRARIES_SEARCH_DIR)
#message ( "GNU_LIBRARIES_SEARCH_DIR=""${GNU_LIBRARIES_SEARCH_DIR}")
#find_library (STDC_LIB stdc++ PATHS ${GNU_LIBRARIES_SEARCH_DIR})
# Méthode 2 : version cmake en pipant les commandes :
execute_process (COMMAND bash "-c" "LANG=C LC_ALL=C ${CMAKE_CXX_COMPILER} -print-search-dirs" COMMAND /usr/bin/sed -n "s/libraries: =//gp" COMMAND tr -s ":" ";" OUTPUT_VARIABLE LIBRARIES_SEARCH_DIR)
find_library (STDC_LIB stdc++ PATHS ${LIBRARIES_SEARCH_DIR})

# Aspects C++ : à affiner si nécessaire avec CMakeDetermineCXXCompiler.cmake
get_filename_component (CXX_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
get_filename_component (CXX_NAME ${CMAKE_CXX_COMPILER} NAME)
get_filename_component (CXX_ROOT_DIR ${CXX_BIN_DIR} DIRECTORY)
message (STATUS "CXX_ROOT_DIR=${CXX_ROOT_DIR} CXX_NAME=${CXX_NAME} CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
message (STATUS "CMAKE_COMPILER_IS_GNUCXX=" ${CMAKE_COMPILER_IS_GNUCXX})

# Ubuntu 17 et 18 : le RUNPATH n'est pas transitif => bien que runpath soit bien positionné (readelf -a executable | grep PATH) les libs utilisées de manière
# indirectes ne sont pas trouvées, et requiert un positionnement de la variable LD_LIBRARY_PATH.
# On force à utiliser un RPATH à l'édition des liens via l'option --disable-new-dtags du linker. A noter que transmise
# à la ligne de commande via -DCMAKE_SHARED_LINKER_FLAGS="-Wl,--disable-new-dtags" cette option est curieusement ignorée.
#execute_process (COMMAND uname -a OUTPUT_VARIABLE FULL_PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
#string (REGEX MATCH "Ubuntu" IS_UBUNTU "${FULL_PLATFORM}")
#string (REGEX MATCH "(17|18)" IS_17_OR_18 "${FULL_PLATFORM}")
#message (STATUS "IS_UBUNTU=${IS_UBUNTU} IS_17_OR_18=${IS_17_OR_18}")
#if (IS_UBUNTU AND IS_17_OR_18)
if (PLATFORM STREQUAL "Ubuntu")
	message (STATUS "PLATFORM UBUNTU 17 OR 18 DETECTED => USING RPATH INSTEADOF RUNPATH (${FULL_PLATFORM}")
	add_link_options ("-Wl,--disable-new-dtags")
endif (PLATFORM STREQUAL "Ubuntu")

if (CMAKE_COMPILER_IS_GNUCXX)
	if ((PLATFORM STREQUAL "CentOS") OR (PLATFORM STREQUAL "RedHat-7-x86_64") OR (PLATFORM STREQUAL "Atos_7__x86_64") OR (PLATFORM STREQUAL "Rhel_8__x86_64"))
# On force le lien à cette version de la libstdc++. C'est nécessaire pour les wrappers swig/python lorsqu'on utilise directement
# le fichier python => chargement par python de _module.so qui, sans cette directive, chargement /lib64/libstdc++.so qui n'aura
# pas une ABI assez récente ...
		message ("==> Ajout de la bibliothèque " ${STDC_LIB} " à l'édition des liens.")
		link_libraries (${STDC_LIB})
		get_filename_component (STDC_LIB_DIR ${STDC_LIB} DIRECTORY)
		message ("==> Ajout de du répertoire " ${STDC_LIB_DIR} " à l'édition des liens.")
#		link_directories (BEFORE "${STDC_LIB_DIR}")
		add_link_options ("-Wl,-rpath,${STDC_LIB_DIR}")		# It works
	endif ( )
	if (PLATFORM STREQUAL "Atos_7__x86_64")	# Malgré include (FindThreads) et target_link_libraries (mylib PUBLIC Threads::Threads) le -lpthread ne suit pas toujours en cmake 3.14
		link_libraries (pthread)
	endif()
elseif (CXX_NAME STREQUAL "icpc")
	# Alors là c'est très fort. Il n'y a que du c++ et pourtant icpc 17.0.4.196 link avec les libs imf et autres, sans -Wl,-rpath
	# Donc on rajoute ça ...
	message (STATUS "CXX IS ICPC")
	find_library (INTEL_LIB intlc PATHS ${LIBRARIES_SEARCH_DIR})
	if (INTEL_LIB)
		get_filename_component (INTEL_LIB_DIR ${INTEL_LIB} DIRECTORY)
		message ("==> Ajout de du répertoire " ${INTEL_LIB_DIR} " à l'édition des liens.")
		add_link_options ("-Wl,-rpath,${INTEL_LIB_DIR}")		# It works
	endif (INTEL_LIB)
endif (CMAKE_COMPILER_IS_GNUCXX)


