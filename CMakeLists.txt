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

# executable and link libraries
ADD_EXECUTABLE( ${PROJECT_NAME} 
    ${DashboardWatch_HEADERS} 
    ${DashboardWatch_SOURCES} 
    ${DashboardWatch_HEADERS_MOC}
    ${DashboardWatch_RCC_SRCS})
    
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES})

