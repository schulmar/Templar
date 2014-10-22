#include "listwidgethandler.h"
#include "traceentry.h"
#include "usedsourcefilemodel.h"
#include "traceentrylist.h"

//#include <QListView>
#include <QTableView>
#include <QSharedPointer>

namespace Templar {

namespace Detail {

class Command
{
public:
    virtual ~Command() {}
    virtual void apply(ListWidgetHandler *handler) = 0;
};


/*
class AddCommand : public Command
{
public:
    AddCommand(QListWidgetItem *item) :
        item(item) {}

    void apply(ListWidgetHandler *handler) {
        handler->addItem(item);
    }
private:
    QListWidgetItem *item;
};
*/


/*class TakeCommand : public Command
{
public:
    void apply(ListWidgetHandler *handler) {
        QSharedPointer<QListWidgetItem> item(handler->takeItem());
        (void)item;
    }
};*/

} // namespace Detail

void ListWidgetHandler::selectRoot(const TraceEntry &entry)
{
    using namespace Detail;

    EntryListModelAdapter *oldModel = dynamic_cast<EntryListModelAdapter*>(proxyModel->sourceModel());
    EntryListModelAdapter *newModel = new EntryListModelAdapter(proxyModel,entry, usedFileModel);

    proxyModel->setSourceModel(newModel);
    delete oldModel;
}

void ListWidgetHandler::inspect(const TraceEntry &entry)
{
     using namespace Detail;

    EntryListModelAdapter *oldModel = dynamic_cast<EntryListModelAdapter*>(proxyModel->sourceModel());
    EntryListModelAdapter *newModel = new EntryListModelAdapter(proxyModel,entry, usedFileModel);

    proxyModel->setSourceModel(newModel);
     delete oldModel;
}


void ListWidgetHandler::handleEvent(const TraceEntry &entry)
{
    using namespace Detail;

    EntryListModelAdapter *oldModel = dynamic_cast<EntryListModelAdapter*>(proxyModel->sourceModel());
    EntryListModelAdapter *newModel = new EntryListModelAdapter(proxyModel,entry, usedFileModel);

     proxyModel->setSourceModel(newModel);
     delete oldModel;
/*    if (entry.isBegin) {
        QListWidgetItem *item = makeItem(entry);
        addItem(item);

        Command *command (new TakeCommand());
        undoList.push_back(command);

    } else {
        QListWidgetItem *item = takeItem();

        Command *command (new AddCommand(item));
        undoList.push_back(command);
    }*/
}

void ListWidgetHandler::undoEvent()
{
    if (undoList.empty())
        return;

    using namespace Detail;

    QSharedPointer<Command> undo (undoList.back());
    undoList.pop_back();

    undo->apply(this);
}

void ListWidgetHandler::reset(const TraceEntry &entry)
{
    handleEvent(entry);
    /*if (navigationHistoryunt() == 0))
        navigationHistoryr();

    if (!undoList.empty())
        undoList.clear();*/
}
/*
void ListWidgetHandler::addItem(QListWidgetItem *item)
{
    navigationHistorytem(item);
    navigationHistoryllToItem(item);
}
*/

void ListWidgetHandler::forward(const std::vector<TraceEntry> &entryVec)
{
    for (unsigned int i = 0; i < entryVec.size(); ++i)
    {
        handleEvent(entryVec[i]);
    }
}

void ListWidgetHandler::rewind(unsigned int count)
{
    while (count--)
    {
        undoEvent();
    }
}

} // namespace Templar
