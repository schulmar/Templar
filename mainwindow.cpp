#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debugmanager.h"
#include "common.h"
#include "highlighter.h"
#include "codeeditor.h"
#include "qgraph.h"
#include "entryvectorbuilder.h"
#include "graphvizbuilder.h"
#include "tracereader.h"
#include "editorhandler.h"
#include "listwidgethandler.h"
#include "entryinfo.h"
#include "entryinfohandler.h"
#include "stringlistdialog.h"
#include "usedsourcefilemodel.h"
#include "traceentry.h"
#include "colorpreferencesdialog.h"

#include "entryfiltersettings.h"
#include "settingsnames.h"

#include <algorithm>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QInputDialog>
#include <QRegExp>
#include <QDockWidget>
//#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QSplitter>
#include <QSettings>

//TODO unused variable: static const int BigGraph = 200;

namespace common { namespace colors {
        QColor nodeColors[10];
        QString colorNames[10] = {
            "Instantiation",
            "Default template argument instantiation",
            "Default function argument instantiation",
            "Explicit argument substitution",
            "Deduced template argument substitution",
            "Prior template argument substitution",
            "Default template argument substitution",
            "Exception specification instantiation",
            "Memoization",
            "Unknown"
        };
}}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      highlighter(nullptr),usedFiles(nullptr)
{
    init();
}

void MainWindow::init()
{
    initGui();
    initManagerAndHandlers();
    makeConnections();
    QSettings settings;

    QVariant lastOpenedTraceFile =
        settings.value(Templar::Settings::Names::last_opened_trace_file);
    if(!lastOpenedTraceFile.isNull())
      openTrace(lastOpenedTraceFile.toString());
}

void MainWindow::initGui() {
    ui->setupUi(this);

    codeEdit = new CodeEditor(this);
    codeEdit->setFont(QFont("Courier"));
    codeEdit->setMinimumHeight(300);

    qGraph = new QGraph(this);

    //listWidget = new QListView(this);
    tableWidget = new QTableView(this);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->verticalHeader()->setDefaultSectionSize(20);
    tableWidget->setSortingEnabled(true);
    connect(tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)), tableWidget, SLOT(sortByColumn(int,)));

    entryProxyModel = new Templar::EntryListSortFilterProxy(this);
    tableWidget->setModel(entryProxyModel);

    fileViewWidget = new QTreeView(this);

    QSplitter *topSplitter = new QSplitter(this);
    topSplitter->addWidget(codeEdit);
    topSplitter->addWidget(qGraph);

    QList<int> sizes;
    sizes << 300 << 300;
    topSplitter->setSizes(sizes);

    QSplitter *bottomSplitter = new QSplitter(this);
//    bottomSplitter->addWidget(entryInfo);
    bottomSplitter->addWidget(fileViewWidget);
    bottomSplitter->addWidget(tableWidget);

    sizes.clear();
    sizes << 400 << 400;
    bottomSplitter->setSizes(sizes);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(topSplitter);
    splitter->addWidget(bottomSplitter);
//    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    sizes.clear();
    sizes << 400 << 100;
    splitter->setSizes(sizes);

    setCentralWidget(splitter);

    // Rewind
    rewindAction = createAction("&Prev","F3", "Rewind", this, ":/icons/rewind");
    ui->mainToolBar->addAction(rewindAction);

    // Previous step
    prevAction = createAction("&Prev","F4", "Previous step", this, ":/icons/back");
    ui->mainToolBar->addAction(prevAction);

    // Next step
    nextAction = createAction("&Next","F5", "Next step", this, ":/icons/forward");
    ui->mainToolBar->addAction(nextAction);

    // Fast Forward step
    forwardAction = createAction("&Next","F6", "Fast Forward", this, ":/icons/fastforward");
    ui->mainToolBar->addAction(forwardAction);

    // follow
    followAction = createAction("&Follow", "", "Follow node", this, ":/icons/follow");
    followAction->setCheckable(true);
    followAction->setChecked(true);
    qGraph->follow(followAction->isChecked());
    ui->mainToolBar->addAction(followAction);

    // separator
    ui->mainToolBar->addSeparator();

    breakpointAction = createAction("&Breakpoint", "Ctrl+B", "Breakpoint", this);
    ui->mainToolBar->addAction(breakpointAction);


    // filter
    filterAction = createAction("&Filter", "Ctrl+F", "Filtering", this);
    ui->mainToolBar->addAction(filterAction);

    // separator
    ui->mainToolBar->addSeparator();

    // reset
    resetAction = createAction("&Reset", "Ctrl+R", "Reset", this);
    ui->mainToolBar->addAction(resetAction);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(new EntryFilterSettings(this));

    listDialog = new StringListDialog("Regexp", this);
}

void MainWindow::initManagerAndHandlers()
{
    using namespace Templar;

    graphHandler = new GraphHandler(qGraph);

    debugManager = new DebugManager(this);
    debugManager->addEventHandler(graphHandler);
    debugManager->addEventHandler(new EditorHandler(codeEdit));
    //debugManager->addEventHandler(new ListWidgetHandler(listWidget));
    debugManager->addEventHandler(new ListWidgetHandler(entryProxyModel));
  //  debugManager->addEventHandler(new EntryInfoHandler(entryInfo));
}

void MainWindow::makeConnections() {
    QObject::connect(nextAction, SIGNAL(triggered()), debugManager, SLOT(next()) );

    QObject::connect(prevAction, SIGNAL(triggered()), debugManager, SLOT(prev()) );

    QObject::connect(forwardAction, SIGNAL(triggered()), debugManager, SLOT(forward()) );

    QObject::connect(rewindAction, SIGNAL(triggered()), debugManager, SLOT(rewind()) );

    QObject::connect(followAction, SIGNAL(triggered()), qGraph, SLOT(follow()) );

    QObject::connect(filterAction, SIGNAL(triggered()), this, SLOT(filterActionClicked()));

    QObject::connect(resetAction, SIGNAL(triggered()), this, SLOT(resetActionClicked()));

    QObject::connect(qGraph, SIGNAL(nodeClicked(QString)), this, SLOT(nodeClicked(QString)));

    QObject::connect(tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(tableWidgetItemClicked(const QModelIndex &)));

    QObject::connect(breakpointAction, SIGNAL(triggered()), this, SLOT(breakpointActionClicked()));

    QObject::connect(fileViewWidget, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(fileWidgetItemClicked(const QModelIndex &)));

  }
void MainWindow::tableWidgetItemClicked(const QModelIndex &index)
{
    using namespace Templar;
    QModelIndex entryModelIndex = entryProxyModel->mapToSource(index);
    TraceEntry *entry = reinterpret_cast<TraceEntry*>(entryModelIndex.internalPointer());

    debugManager->selectRoot(*entry);
}
void MainWindow::fileWidgetItemClicked(const QModelIndex &index)
{
    using namespace Templar;

    const SourceFileNode *selectedNode = static_cast<const SourceFileNode*>(index.internalPointer());
    if(selectedNode->id!=std::numeric_limits<size_t>::max())
    {
        debugManager->gotoFile(selectedNode->id);
    }
}

void MainWindow::breakpointActionClicked()
{
    listDialog->setDefaultValue(QRegExp::escape(activeNode));
    listDialog->exec();

    QStringList stringList = listDialog->getStringList();

    QList<QRegExp> regexpList;

    for(int i = 0; i < stringList.count(); ++i)
    {
        regexpList.append(QRegExp(stringList.at(i)));
    }

    debugManager->setBreakpoints(regexpList);
}

void MainWindow::reset() {
    if (currentFileName == "")
        return;

    using namespace Templar;

    QString dirPath = currentFileName.left(currentFileName.lastIndexOf("/") + 1);

    TraceReader reader(debugManager->getEntryTarget());
    reader.setDirPath(dirPath);

    GraphvizBuilder graphvizBuilder;
    EntryVectorBuilder vecBuilder;

    reader.build(currentFileName);

    debugManager->reset();

    graphHandler->setGvc(graphvizBuilder.getGvc());
    graphHandler->setNodeGraphMap(graphvizBuilder.getNodeGraphMap());
}

void MainWindow::filterActionClicked()
{
    bool ok;
    static QString last;
    QString text = QInputDialog::getText(this, tr("Filter Nodes"),
                                         tr("Enter RegExp:"), QLineEdit::Normal,
                                         activeNode.isEmpty() ? last : QRegExp::escape(activeNode), &ok);
    if (ok && text != last){
        QRegExp regexp(text);
        ignoreList.append(regexp);
        last = text;
        reset();
    }
}

void MainWindow::nodeClicked(const QString& node)
{
    activeNode = node;

    showInformation(activeNode);
}

void MainWindow::resetActionClicked()
{
    ignoreList.clear();
    reset();
}

MainWindow::~MainWindow()
{
    QSettings settings(QSettings::UserScope,"Templar2","Templar2");
    settings.beginWriteArray("nodeColors");
    for(int i=0;i<10;i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("color",common::colors::nodeColors[i].name());
    }
    delete graphHandler;
    delete ui;
}

QAction* MainWindow::createAction(const char* name, const char* shortcut, const char* statusTip, QWidget* parent, QString icon)
{
    QAction* action = new QAction(tr(name), parent);
    action->setIcon(QIcon(icon));
    action->setShortcut(tr(shortcut));
    action->setStatusTip(tr(statusTip));
    return action;
}

QAction* MainWindow::createAction(const char* name, const char* shortcut, const char* statusTip, QWidget* parent)
{
    QAction* action = new QAction(tr(name), parent);
    action->setShortcut(tr(shortcut));
    action->setStatusTip(tr(statusTip));
    return action;
}

void MainWindow::on_actionOpen_trace_triggered()
{
    QString fileFilters = tr("Trace files (*.trace.xml);; Any files (*)");
    QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open trace file"),
            currentFileName.left(
                currentFileName.lastIndexOf(QDir::separator())
            ), fileFilters );

    if (fileName.isEmpty())
        return;
    openTrace(fileName);

}
void MainWindow::openTrace(const QString &fileName)
{
    currentFileName = fileName;
    ignoreList.clear();


    QString srcFilename = currentFileName.left(currentFileName.lastIndexOf(".memory.trace.xml"));
    usedFiles = new Templar::UsedSourceFileModel(srcFilename+".filelist.trace");
    debugManager->setUsedFileModel(usedFiles);

    QObject::connect(
                usedFiles,SIGNAL(dataChanged(QModelIndex,QModelIndex))
                ,entryProxyModel,SLOT(fileFilterDataChanged(const QModelIndex&,const QModelIndex&))
            );

    QFile file(srcFilename);
    file.open(QIODevice::ReadOnly);

    QString source(file.readAll());
    source.replace("\t", "    ");

    codeEdit->setPlainText(source);
    highlighter = new Highlighter(codeEdit->document());

    fileViewWidget->setModel(usedFiles);
    try {
      reset();
    } catch (Templar::FileException *) {
      QMessageBox::warning(this, tr("Error"), tr("Can't open trace file"));
      return;
    } catch (...) {
      QMessageBox::warning(this, tr("Error"),
                           tr("Can't build graph from trace file"));
      return;
    }
    QSettings().setValue(Templar::Settings::Names::last_opened_trace_file,
                         fileName);
    showInformation();
}

void MainWindow::showInformation(QString info)
{
    statusBar()->showMessage(tr("Number of trace events: ")
                             + QString::number(debugManager->getEventCount())
                             + " | " + info);
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionDot_triggered()
{
    setLayout("dot");
}

void MainWindow::on_actionCirco_triggered()
{
    setLayout("circo");
}

void MainWindow::on_actionNeato_triggered()
{
    setLayout("neato");
}

void MainWindow::on_actionFdp_triggered()
{
    setLayout("fdp");
}

void MainWindow::on_actionTwopi_triggered()
{
    setLayout("twopi");
}

void MainWindow::setLayout(QString /*layout*/){

}

void MainWindow::on_actionExport_to_PNG_triggered()
{
    QString fileFilters = tr("Png files (*.png)");

    QString fileName = QFileDialog::getSaveFileName(this,tr("Export to PNG"),
        qApp->applicationDirPath() + QDir::separator()
                + "traces" + QDir::separator(), fileFilters);
    if (fileName.isEmpty()) return;

    if (!fileName.contains(".png")) fileName += ".png";

    /*if (model->size() <= BigGraph){
        if (gvLayout(model->getGVC(), model->getGraph(), currentLayout.toUtf8().data()) == 0)
        {
              gvRenderFilename(model->getGVC(), model->getGraph(),
                         "png", fileName.toUtf8().data() );

              gvFreeLayout(model->getGVC(), model->getGraph());
              return;
         }
    }*/

    QGraphicsScene *scene = qGraph->getScene();
    int width = scene->width(),
        height = scene->height();

    QImage image(width, height, QImage::Format_RGB16);
    if ( !image.isNull()){
        exportToPNG(&image, scene, fileName );
        return;
    }

    QImage imageMono(width, height, QImage::Format_Mono);
    if (!imageMono.isNull()){
        exportToPNG(&imageMono, scene, fileName);
        return;
    }

    QMessageBox::warning(this, tr("Error"), tr("Can't export file to PNG"));
}

void MainWindow::exportToPNG(QImage *image, QGraphicsScene* scene, const QString& fileName)
{
    QPainter painter(image);
    scene->render(&painter);

    image->save(fileName,"PNG");

    showInformation("graph exported to png");
}

void MainWindow::on_actionNode_Colors_triggered()
{
    ColorPreferencesDialog dlg(this);
    dlg.exec();
}
