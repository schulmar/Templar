#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
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
    class UsedSourceFileModel;
    class EntryListSortFilterProxy;
}

//class Agnode_t;
class QImage;
class QGraphicsScene;
class Highlighter;
class CodeEditor;
class QGraph;
class QModelIndex;
class QTableView;
class QTreeView;
class EntryInfo;
class StringListDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExport_to_PNG_triggered();
    void on_actionTwopi_triggered();
    void on_actionFdp_triggered();
    void on_actionNeato_triggered();
    void on_actionCirco_triggered();
    void on_actionDot_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_trace_triggered();

    void nodeClicked(const QString& node);

    void filterActionClicked();
    void resetActionClicked();
    void breakpointActionClicked();

    //void listWidgetItemClicked(QListWidgetItem *item);
    void tableWidgetItemClicked(const QModelIndex & index);
    void fileWidgetItemClicked(const QModelIndex &index);

    void on_actionNode_Colors_triggered();

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
    void openTrace(const QString &fileName);
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
    std::unique_ptr<Templar::UsedSourceFileModel> usedFiles;
    Templar::EntryListSortFilterProxy *entryProxyModel;

    //QListView *listWidget;
    QTableView *tableWidget;
    QTreeView *fileViewWidget;

    QList<QRegExp> ignoreList;
};

#endif // MAINWINDOW_H
