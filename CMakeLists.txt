# start
cmake_minimum_required (VERSION 2.6)
project (DashboardWatch)

# qt
FIND_PACKAGE( Qt4 REQUIRED )
SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# sources
SET(DashboardWatch_SOURCES main.cpp DashboardWatch.cpp)
SET(DashboardWatch_HEADERS DashboardWatch.h)
SET(DashboardWatch_RCCS resources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(DashboardWatch_RCC_SRCS ${DashboardWatch_RCCS})
QT4_WRAP_CPP(DashboardWatch_HEADERS_MOC ${DashboardWatch_HEADERS})

# Set some Win32 Specific Settings
IF(WIN32)
  SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
  SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

# executable and link libraries
ADD_EXECUTABLE( ${PROJECT_NAME} ${GUI_TYPE}
    ${DashboardWatch_HEADERS} 
    ${DashboardWatch_SOURCES} 
    ${DashboardWatch_HEADERS_MOC}
    ${DashboardWatch_RCC_SRCS})
    
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES})

# cpack
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)

IF(WIN32)
  SET(QT_DLLS "${QT_LIBRARY_DIR}/bin/QtCore.dll" ${QT_LIBRARY_DIR}/bin/QtGui.dll" ${QT_LIBRARY_DIR}/bin/QtNetwork.dll")
  INSTALL(FILES ${QT_DLLS} DESTINATION bin)
  SET(CPACK_GENERATOR ZIP)
  SET(CPACK_SOURCE_GENERATOR ZIP)
ELSE(WIN32)
  SET(CPACK_GENERATOR TGZ)
  SET(CPACK_SOURCE_GENERATOR TGZ)
ENDIF(WIN32)

INCLUDE(CPack)
