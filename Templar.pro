#-------------------------------------------------
#
# Project created by QtCreator 2012-12-29T15:55:41
#
#-------------------------------------------------

INCLUDEPATH += /home/ezoltbo/prefix/include

LIBS += -L/home/ezoltbo/prefix/lib \
    -lgvc \
    -lgraph \
    -lcdt

QT += \
    xml \
    xmlpatterns \
    core gui

TARGET = Templar
TEMPLATE = app

CONFIG += debug

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
    qgraph.cpp

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
    qgraph.h

FORMS    += mainwindow.ui \
    sourceviewer.ui \
    entryinfo.ui \
    stringlistdialog.ui

RESOURCES += \
    resources.qrc
