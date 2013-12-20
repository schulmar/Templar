#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QPointer>
#include <QMap>
#include <QRegExp>

#include <graphviz/gvc.h>

#include "graphhandler.h"

namespace Ui
{
    class MainWindow;
}

namespace Templar{
    class DebugManager;
}

class Agnode_t;
class QImage;
class QGraphicsScene;
class Highlighter;
class CodeEditor;
class QGraph;
class QListWidget;
class QListWidgetItem;
class EntryInfo;
class StringListDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExport_to_PNG_activated();
    void on_actionTwopi_activated();
    void on_actionFdp_activated();
    void on_actionNeato_activated();
    void on_actionCirco_activated();
    void on_actionDot_activated();
    void on_actionExit_activated();
    void on_actionOpen_trace_activated();

    void nodeClicked(const QString& node);

    void filterActionClicked();
    void resetActionClicked();
    void breakpointActionClicked();

    void listWidgetItemClicked(QListWidgetItem *item);

private:
    void init();
    void initGui();
    void initManagerAndHandlers();
    void makeConnections();

    void reset();
    QAction* createAction(const char* name, const char* shortcut, const char* statusTip, QWidget* parent, QString icon);
    QAction* createAction(const char* name, const char* shortcut, const char* statusTip, QWidget* parent=0);

    void showInformation(QString info = "");
    void setLayout(QString layout);
    void exportToPNG(QImage*, QGraphicsScene*, const QString&);

private:
    Ui::MainWindow *ui;

    QAction* nextAction;
    QAction* prevAction;

    QAction* forwardAction;
    QAction* rewindAction;

    QAction* followAction;

    QAction* breakpointAction;

    QAction* filterAction;
    QAction* resetAction;

    Templar::DebugManager *debugManager;
    QString currentLayout;
    QString currentFileName;
    QString activeNode;

    StringListDialog *listDialog;

    CodeEditor *codeEdit;
    EntryInfo *entryInfo;
    QGraph *qGraph;

    Templar::GraphHandler *graphHandler;

    QPointer<Highlighter> highlighter;
    QMap<QString, QString> openedFiles;

    QListWidget *listWidget;

    QList<QRegExp> ignoreList;
};

#endif // MAINWINDOW_H
