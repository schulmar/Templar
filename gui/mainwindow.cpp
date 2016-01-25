#include "../gui/mainwindow.h"

#include "ui_mainwindow.h"
#include "debugmanager.h"
#include "common.h"
#include "highlighter.h"
#include "tracereaders/tracereader.h"
#include "templateeventhandlers/editorhandler.h"
#include "templateeventhandlers/listwidgethandler.h"
#include "entryinfo.h"
#include "templateeventhandlers/entryinfohandler.h"
#include "usedsourcefilemodel.h"
#include "traceentry.h"

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
#include <QFileInfo>
#include <QDebug>
#include "../gui/codeeditor.h"
#include "../gui/colorpreferencesdialog.h"
#include "../gui/entryfiltersettings.h"
#include "../gui/qgraph.h"
#include "../gui/stringlistdialog.h"
#include "traceentrylistfilterproxy.hpp"

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
    fileViewWidget->setHeaderHidden(true);

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

    upAction = createAction("&Up", "F7", "Up", this, ":/icons/up");
    ui->mainToolBar->addAction(upAction);

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

    QObject::connect(upAction, SIGNAL(triggered()), this, SLOT(upActionClicked()));

    QObject::connect(qGraph, SIGNAL(nodeClicked(QString)), this, SLOT(nodeClicked(QString)));

    QObject::connect(qGraph, SIGNAL(nodeDoubleClicked(int)), this,
                     SLOT(nodeDoubleClicked(int)));

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

    debugManager->getEntryTarget().clear();
    usedFiles = TraceReader::build(currentFileName, debugManager->getEntryTarget(), dirPath);
    QString failingRelativePath;
    while (!usedFiles->checkRelativePathRoot(&failingRelativePath)) {
      auto result = QMessageBox::question(
          this, tr("Source files not found"),
          tr("The source file %0 doesn't seem to lie at %1, do "
             "you want to set another path?")
              .arg(failingRelativePath)
              .arg(usedFiles->relativePathRoot),
          QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
          QMessageBox::StandardButton::Yes);
      if(result ==  QMessageBox::StandardButton::No) {
    	  break;
      } else {
        auto root = QFileDialog::getExistingDirectory(
            this,
            tr("Select root for relative path %0").arg(failingRelativePath),
            usedFiles->relativePathRoot);
        if (root.isEmpty()) {
          break;
        } else {
          usedFiles->relativePathRoot = root;
        }
      }
    }
    QObject::connect(
        usedFiles.get(), SIGNAL(dataChanged(QModelIndex, QModelIndex)),
        entryProxyModel,
        SLOT(fileFilterDataChanged(const QModelIndex &, const QModelIndex &)));
    debugManager->setUsedFileModel(usedFiles.get());
    fileViewWidget->setModel(usedFiles.get());
    entryProxyModel->setUsedSourceFileModel(usedFiles.get());

    debugManager->reset();
    debugManager->selectRoot(debugManager->getEntryTarget());
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

void MainWindow::nodeDoubleClicked(int entryId) {
	auto entry = debugManager->getEntryById(entryId);
	if(entry) {
		debugManager->selectRoot(*entry);
    } else {
        QMessageBox::warning(this, "Invalid entry id",
                             "There is no entry with the id that you selected. "
                             "This should not happen, please notify the "
                             "maintainer of this program.");
    }
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
    action->setShortcut(QKeySequence(tr(shortcut)));
    action->setStatusTip(tr(statusTip));
    return action;
}

QAction* MainWindow::createAction(const char* name, const char* shortcut, const char* statusTip, QWidget* parent)
{
    QAction* action = new QAction(tr(name), parent);
    action->setShortcut(QKeySequence(tr(shortcut)));
    action->setStatusTip(tr(statusTip));
    return action;
}

void MainWindow::on_actionOpen_trace_triggered()
{
    QString fileFilters = tr("Trace files (*.trace.xml"
#if YAML_TRACEFILE_SUPPORT
                              " *.trace.yaml"
#endif
                              " *.trace.pbf"
                              ");; Any files (*)");
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

    if (!QFileInfo(fileName).exists())
        return;

    QString srcFilename = Templar::sourceFileNameFromTraceFileName(fileName);

    QFile file(srcFilename);
    file.open(QIODevice::ReadOnly);

    QString source(file.readAll());
    source.replace("\t", "    ");

    codeEdit->setPlainText(source);
    highlighter = new Highlighter(codeEdit->document());
    try {
      reset();
    } catch (Templar::FileException *) {
      QMessageBox::warning(this, tr("Error"), tr("Can't open trace file"));
      return;
    } catch (std::runtime_error const &error) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Could not load trace file:\n") + error.what());
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

void MainWindow::upActionClicked() {
	debugManager->selectParentRoot();
	auto entry = debugManager->getCurrentEntry();
	if(entry) {
		qGraph->selectEntry(entry->id);
	}
}
