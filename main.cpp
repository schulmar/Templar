#include <QApplication>
#include <QSettings>
#include <QColor>
#include "mainwindow.h"
#include "common.h"
int main(int argc, char *argv[])
{
    QSettings settings(QSettings::UserScope,"Templar2","Templar2");
    settings.beginReadArray("nodeColors");
    for(int i=0;i<10;i++)
    {
        settings.setArrayIndex(i);
        common::colors::nodeColors[i] = QColor(settings.value("color",Qt::black).toString());

    }
    settings.endArray();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
