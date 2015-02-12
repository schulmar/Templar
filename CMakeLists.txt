cmake_minimum_required(VERSION 2.8)

project(Templar)

option(DONT_USE_QT5 "Disables Qt5 even if present on the system" OFF)

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


find_package(Qt5Widgets)
if(NOT Qt5Widgets_FOUND OR DONT_USE_QT5)
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
  "templateeventhandler.h"
  "listwidgethandler.h"
  "highlighter.h"
  "graphvizbuilder.h"
  "graphhandler.h"
  "entryvectorbuilder.h"
  "entryinfo.h"
  "editorhandler.h"
  "debugmanager.h"
  "common.h"
  "builder.h"
  "entryinfohandler.h"
  "usedsourcefilemodel.h"
  "settingsnames.h"
  "traceentrylist.h"
  
  "gui/mainwindow.h"
  "gui/sourceviewer.h"
  "gui/codeeditor.h"
  "gui/stringlistdialog.h"
  "gui/qgraph.h"
  "gui/entryfiltersettings.h"
  "gui/colorpreferencesdialog.h"
  
  "tracereaders/tracereader.h"
  "tracereaders/oldxmltracereader.h"
  "tracereaders/protobuftracereader.h"
  "tracereaders/binarytracereader.h"
)
set(TEMPLAR_SOURCES ${TEMPLAR_SOURCES} 
  "main.cpp"
  "templateeventhandler.cpp"
  "listwidgethandler.cpp"
  "highlighter.cpp"
  "graphvizbuilder.cpp"
  "graphhandler.cpp"
  "entryvectorbuilder.cpp"
  "entryinfo.cpp"
  "editorhandler.cpp"
  "debugmanager.cpp"
  "builder.cpp"
  "entryinfohandler.cpp"
  "traceentry.cpp"
  "usedsourcefilemodel.cpp"
  "settingsnames.cpp"
  "traceentrylist.cpp"
  
  "gui/mainwindow.cpp"
  "gui/sourceviewer.cpp"
  "gui/codeeditor.cpp"
  "gui/stringlistdialog.cpp"
  "gui/qgraph.cpp"
  "gui/entryfiltersettings.cpp"
  "gui/colorpreferencesdialog.cpp"
  
  "tracereaders/tracereader.cpp"
  "tracereaders/oldxmltracereader.cpp"
  "tracereaders/protobuftracereader.cpp"
  "tracereaders/binarytracereader.cpp"
)

set(TEMPLAR_RESOURCES
  "resources.qrc"
)

if(USING_QT5)
  QT5_WRAP_CPP( TEMPLAR_MOC_SRCS ${TEMPLAR_MOC_HDRS} )# OPTIONS "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED")
  QT5_WRAP_UI( TEMPLAR_UI_HDRS ${TEMPLAR_UIS} )
  QT5_ADD_RESOURCES( TEMPLAR_RESOURCES_RCC ${TEMPLAR_RESOURCES} )
else()
  QT4_WRAP_UI( TEMPLAR_UI_HDRS ${TEMPLAR_UIS} )
  QT4_WRAP_CPP( TEMPLAR_MOC_SRCS ${TEMPLAR_MOC_HDRS} )# OPTIONS "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED")
  QT4_ADD_RESOURCES( TEMPLAR_RESOURCES_RCC ${TEMPLAR_RESOURCES} )
endif()

add_executable(Templar ${TEMPLAR_SOURCES} ${TEMPLAR_MOC_SRCS} ${TEMPLAR_UI_HDRS} ${TEMPLAR_RESOURCES_RCC})
target_link_libraries(Templar ${QT_LIBRARIES} ${EXTRA_LIBS})









