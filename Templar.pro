#-------------------------------------------------
#
# Project created by QtCreator 2012-12-29T15:55:41
#
#-------------------------------------------------

INCLUDEPATH += 
LIBS += -lgvc \
    -lcgraph \
    -lcdt

QT += core gui
#check for Qt5
contains(QT_VERSION, ^5\\..*) {
QT += widgets
DEFINES+=USE_QT5
}

TARGET = Templar
TEMPLATE = app

CONFIG += debug

QMAKE_CXXFLAGS += -std=c++11

DEFINES += YAML_TRACEFILE_SUPPORT=1
LIBS += -lyaml-cpp
SOURCES += yamltracereader.cpp
HEADERS += yamltracereader.h

SOURCES += main.cpp\
        mainwindow.cpp \
    tracereader.cpp \
    templateeventhandler.cpp \
    sourceviewer.cpp \
    listwidgethandler.cpp \
    highlighter.cpp \
    graphvizbuilder.cpp \
    graphhandler.cpp \
    entryvectorbuilder.cpp \
    entryinfo.cpp \
    editorhandler.cpp \
    debugmanager.cpp \
    codeeditor.cpp \
    builder.cpp \
    entryinfohandler.cpp \
    stringlistdialog.cpp \
    qgraph.cpp \
    traceentry.cpp \
    usedsourcefilemodel.cpp \
    entryfiltersettings.cpp \
    colorpreferencesdialog.cpp \
    settingsnames.cpp \
    oldxmltracereader.cpp \
    protobuftracereader.cpp \
    binarytracereader.cpp \
    traceentrylist.cpp

HEADERS  += mainwindow.h \
    tracereader.h \
    traceentry.h \
    templateeventhandler.h \
    sourceviewer.h \
    listwidgethandler.h \
    highlighter.h \
    graphvizbuilder.h \
    graphhandler.h \
    entryvectorbuilder.h \
    entryinfo.h \
    editorhandler.h \
    debugmanager.h \
    common.h \
    codeeditor.h \
    builder.h \
    entryinfohandler.h \
    stringlistdialog.h \
    qgraph.h \
    usedsourcefilemodel.h \
    entryfiltersettings.h \
    colorpreferencesdialog.h \
    settingsnames.h \
    oldxmltracereader.h \
    protobuftracereader.h \
    binarytracereader.h \
    traceentrylist.h

FORMS    += mainwindow.ui \
    sourceviewer.ui \
    entryinfo.ui \
    stringlistdialog.ui \
    entryfiltersettings.ui \
    colorpreferencesdialog.ui

RESOURCES += \
    resources.qrc
