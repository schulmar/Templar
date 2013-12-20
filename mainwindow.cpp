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

#include <algorithm>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QInputDialog>
#include <QRegExp>
#include <QDockWidget>
#include <QListWidget>
#include <QSplitter>

static const int BigGraph = 200;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      highlighter(0)
{
    init();
}

void MainWindow::init()
{
    initGui();
    initManagerAndHandlers();
    makeConnections();
}

void MainWindow::initGui() {
    ui->setupUi(this);

    codeEdit = new CodeEditor(this);
    codeEdit->setFont(QFont("Courier"));
    codeEdit->setMinimumHeight(300);

    qGraph = new QGraph(this);

    listWidget = new QListWidget(this);

    entryInfo = new EntryInfo(this);

    QSplitter *topSplitter = new QSplitter(this);
    topSplitter->addWidget(codeEdit);
    topSplitter->addWidget(qGraph);

    QList<int> sizes;
    sizes << 300 << 300;
    topSplitter->setSizes(sizes);

    QSplitter *bottomSplitter = new QSplitter(this);
    bottomSplitter->addWidget(entryInfo);
    bottomSplitter->addWidget(listWidget);

    sizes.clear();
    sizes << 400 << 100;
    bottomSplitter->setSizes(sizes);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(topSplitter);
    splitter->addWidget(bottomSplitter);

    sizes.clear();
    sizes << 400 << 200;
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

    listDialog = new StringListDialog("Regexp", this);
}

void MainWindow::initManagerAndHandlers()
{
    using namespace Templar;

    graphHandler = new GraphHandler(qGraph);

    debugManager = new DebugManager(this);
    debugManager->addEventHandler(graphHandler);
    debugManager->addEventHandler(new EditorHandler(codeEdit));
    debugManager->addEventHandler(new ListWidgetHandler(listWidget));
    debugManager->addEventHandler(new EntryInfoHandler(entryInfo));
}

void MainWindow::makeConnections() {
    QObject::connect(nextAction, SIGNAL(triggered()), debugManager, SLOT(next()) );

    QObject::connect(prevAction, SIGNAL(triggered()), debugManager, SLOT(prev()) );

    QObject::connect(forwardAction, SIGNAL(triggered()), debugManager, SLOT(forward()) );

    QObject::connect(rewindAction, SIGNAL(triggered()), debugManager, SLOT(rewind()) );

    QObject::connect(followAction, SIGNAL(triggered(bool)), qGraph, SLOT(follow(bool)) );

    QObject::connect(filterAction, SIGNAL(triggered()), this, SLOT(filterActionClicked()));

    QObject::connect(resetAction, SIGNAL(triggered()), this, SLOT(resetActionClicked()));

    QObject::connect(qGraph, SIGNAL(nodeClicked(QString)), this, SLOT(nodeClicked(QString)));

    QObject::connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listWidgetItemClicked(QListWidgetItem*)));

    QObject::connect(breakpointAction, SIGNAL(triggered()), this, SLOT(breakpointActionClicked()));
}

void MainWindow::listWidgetItemClicked(QListWidgetItem *item)
{
    using namespace Templar;
    TraceEntry entry = item->data(Qt::UserRole).value<TraceEntry>();

    debugManager->inspect(entry);
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

    TraceReader reader;
    reader.setDirPath(dirPath);
    reader.setIgnoreList(ignoreList);

    GraphvizBuilder graphvizBuilder;
    EntryVectorBuilder vecBuilder;

    reader.addBuilder(&graphvizBuilder);
    reader.addBuilder(&vecBuilder);

    reader.build(currentFileName);

    debugManager->reset(vecBuilder.getTraceEntryVector());

    graphHandler->setGvc(graphvizBuilder.getGvc());
    graphHandler->setNodeGraphMap(graphvizBuilder.getNodeGraphMap());
}

void MainWindow::filterActionClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Filter Nodes"),
                                         tr("Enter RegExp:"), QLineEdit::Normal,
                                         QRegExp::escape(activeNode), &ok);
    if (ok && !text.isEmpty()){
        QRegExp regexp(text);
        ignoreList.append(regexp);

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

void MainWindow::on_actionOpen_trace_activated()
{
    QString fileFilters = tr("Trace files (*.trace.xml);; Any files (*)");
    QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open trace file"),
            currentFileName.left(
                currentFileName.lastIndexOf(QDir::separator())
            ), fileFilters );

    if (fileName.isEmpty())
        return;

    currentFileName = fileName;

    try {
        ignoreList.clear();
        reset();
    } catch (Templar::FileException*) {
        QMessageBox::warning(this, tr("Error"), tr("Can't open trace file"));
        return;
    } catch (...) {
        QMessageBox::warning(this, tr("Error"), tr("Can't build graph from trace file"));
        return;
    }

    QString srcFilename = currentFileName.left(currentFileName.lastIndexOf(".trace.xml"));
    QFile file(srcFilename);
    file.open(QIODevice::ReadOnly);

    QString source(file.readAll());
    source.replace("\t", "    ");

    codeEdit->setPlainText(source);
    highlighter = new Highlighter(codeEdit->document());

    showInformation();
}

void MainWindow::showInformation(QString info)
{
    statusBar()->showMessage(tr("Number of trace events: ")
                             + QString::number(debugManager->getEventCount())
                             + " | " + info);
}

void MainWindow::on_actionExit_activated()
{
    qApp->quit();
}

void MainWindow::on_actionDot_activated()
{
    setLayout("dot");
}

void MainWindow::on_actionCirco_activated()
{
    setLayout("circo");
}

void MainWindow::on_actionNeato_activated()
{
    setLayout("neato");
}

void MainWindow::on_actionFdp_activated()
{
    setLayout("fdp");
}

void MainWindow::on_actionTwopi_activated()
{
    setLayout("twopi");
}

void MainWindow::setLayout(QString /*layout*/){

}

void MainWindow::on_actionExport_to_PNG_activated()
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
