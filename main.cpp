#include <QApplication>
#include <QSettings>
#include <QColor>
#include "gui/mainwindow.h"
#include "common.h"
#include "settingsnames.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Templar::Settings::OrganizationName);
    QCoreApplication::setApplicationName(Templar::Settings::ApplicationName);
    QSettings settings;
    settings.beginReadArray("nodeColors");
    for(int i=0;i<10;i++)
    {
        settings.setArrayIndex(i);
        common::colors::nodeColors[i] =
            QColor(settings.value("color", QColor(Qt::black)).toString());
    }
    settings.endArray();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
