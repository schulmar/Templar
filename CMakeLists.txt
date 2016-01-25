cmake_minimum_required(VERSION 3.0)

project(Templar)

option(DONT_USE_QT5 "Disables Qt5 even if present on the system" OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON) 
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Configured for build-type: ${CMAKE_BUILD_TYPE}")

include(CheckCXXCompilerFlag)

# Check and enable C++11 or C++0x features:
if(NOT MSVC)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" _COMPILER_SUPPORTS_CXX11)
  if(_COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" _COMPILER_SUPPORTS_CXX0X)
    if(_COMPILER_SUPPORTS_CXX0X)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
  endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 -D_SCL_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Woverloaded-virtual -Wold-style-cast -Wnon-virtual-dtor")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  set(EXTRA_LIBS "-lpthread")
  if (WIN32)
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --enable-stdcall-fixup")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  endif()
endif()
message(STATUS "Configured compiler options for ${CMAKE_SYSTEM_NAME} system with ${CMAKE_CXX_COMPILER_ID} toolset.")

if(NOT DONT_USE_QT5)
	find_package(Qt5Widgets)
endif()
if(NOT Qt5Widgets_FOUND)
    if(DONT_USE_QT5)
        message("Ignoring Qt5 and using Qt4 instead as requested.")
    else()
        message("Qt5 could not be found, falling back to Qt4")
    endif()
    
    find_package( Qt4 REQUIRED QtCore QtGui )
    
    include( ${QT_USE_FILE} )
    add_definitions(${QT_DEFINITIONS})
    
else()
    find_package(Qt5Core REQUIRED)
    set(USING_QT5 TRUE)
    message("Using Qt5")
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})
    add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Core_DEFINITIONS} -DUSE_QT5=1)
    set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})
    
endif()


find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp)
if(YamlCpp_FOUND)
  add_definitions(-DYAML_TRACEFILE_SUPPORT=1)
  include_directories(${YamlCpp_INCLUDE_DIRS})
  set(EXTRA_LIBS ${EXTRA_LIBS} ${YamlCpp_LIBRARIES})
  set(TEMPLAR_SOURCES ${TEMPLAR_SOURCES} "tracereaders/yamltracereader.cpp")
  set(TEMPLAR_MOC_HDRS ${TEMPLAR_MOC_HDRS} "tracereaders/yamltracereader.h")
endif()

find_library(GVC_LIB gvc)
if(NOT GVC_LIB)
  message(FATAL_ERROR "Could not locate the required library: 'libgvc.so'")
endif()

find_library(CGRAPH_LIB cgraph)
if(NOT CGRAPH_LIB)
  message(FATAL_ERROR "Could not locate the required library: 'libcgraph.so'")
endif()

find_library(CDT_LIB cdt)
if(NOT CDT_LIB)
  message(FATAL_ERROR "Could not locate the required library: 'libcdt.so'")
endif()

set(EXTRA_LIBS ${EXTRA_LIBS} ${GVC_LIB} ${CGRAPH_LIB} ${CDT_LIB})

# So that the moc'd headers and sources can be found, and also, the protobuf source and headers:
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${PROTOBUF_LIBRARIES})
  include_directories(${PROTOBUF_INCLUDE_DIRS})
  
  set(TEMPLAR_PROTOS 
    "templight_messages.proto"
  )
  
  message("Generating protocol buffer classes from .proto files.")
  
  PROTOBUF_GENERATE_CPP(TEMPLAR_PROTO_SRCS TEMPLAR_PROTO_HDRS ${TEMPLAR_PROTOS})
  
  set(TEMPLAR_SOURCES ${TEMPLAR_SOURCES} ${TEMPLAR_PROTO_SRCS})
  set(TEMPLAR_MOC_HDRS ${TEMPLAR_MOC_HDRS} ${TEMPLAR_PROTO_HDRS})
  
endif()



set( TEMPLAR_UIS
  "gui/mainwindow.ui"
  "gui/sourceviewer.ui"
  "gui/entryinfo.ui"
  "gui/stringlistdialog.ui"
  "gui/entryfiltersettings.ui"
  "gui/colorpreferencesdialog.ui"
)

set( TEMPLAR_MOC_HDRS ${TEMPLAR_MOC_HDRS} 
  "traceentry.h"
  "common.h"
  "usedsourcefilemodel.h"
  "settingsnames.h"
  "traceentrylistfilterproxy.hpp"
  
  "gui/debugmanager.h"
  "gui/highlighter.h"
  "gui/entryinfo.h"
  "gui/mainwindow.h"
  "gui/sourceviewer.h"
  "gui/codeeditor.h"
  "gui/stringlistdialog.h"
  "gui/qgraph.h"
  "gui/entryfiltersettings.h"
  "gui/colorpreferencesdialog.h"
  
  "templateeventhandlers/traceentrylist.h"
  "templateeventhandlers/graphvizvisitor.h"
  "templateeventhandlers/templateeventhandler.h"
  "templateeventhandlers/listwidgethandler.h"
  "templateeventhandlers/graphhandler.h"
  "templateeventhandlers/editorhandler.h"
  "templateeventhandlers/entryinfohandler.h"
  
  "tracereaders/tracereader.h"
  "tracereaders/oldxmltracereader.h"
  "tracereaders/protobuftracereader.h"
  "tracereaders/binarytracereader.h"
)
set(TEMPLAR_SOURCES ${TEMPLAR_SOURCES}
  "main.cpp"
  "traceentry.cpp"
  "usedsourcefilemodel.cpp"
  "settingsnames.cpp"
  "traceentrylistfilterproxy.cpp"
  
  "helper/breakstring.cpp"
  
  "gui/debugmanager.cpp"
  "gui/highlighter.cpp"
  "gui/entryinfo.cpp"
  "gui/mainwindow.cpp"
  "gui/sourceviewer.cpp"
  "gui/codeeditor.cpp"
  "gui/stringlistdialog.cpp"
  "gui/qgraph.cpp"
  "gui/entryfiltersettings.cpp"
  "gui/colorpreferencesdialog.cpp"
  
  "templateeventhandlers/traceentrylist.cpp"
  "templateeventhandlers/graphvizvisitor.cpp"
  "templateeventhandlers/templateeventhandler.cpp"
  "templateeventhandlers/listwidgethandler.cpp"
  "templateeventhandlers/graphhandler.cpp"
  "templateeventhandlers/editorhandler.cpp"
  "templateeventhandlers/entryinfohandler.cpp"
  
  "tracereaders/tracereader.cpp"
  "tracereaders/oldxmltracereader.cpp"
  "tracereaders/protobuftracereader.cpp"
  "tracereaders/binarytracereader.cpp"
)

set(TEMPLAR_RESOURCES
  "resources.qrc"
)

add_executable(Templar ${TEMPLAR_SOURCES} ${TEMPLAR_RESOURCES})
target_link_libraries(Templar ${QT_LIBRARIES} ${EXTRA_LIBS})









