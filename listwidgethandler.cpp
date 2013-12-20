#include "listwidgethandler.h"
#include "traceentry.h"

#include <QListWidget>
#include <QSharedPointer>

namespace Templar {

namespace Detail {

class Command
{
public:
    virtual void apply(ListWidgetHandler *handler) = 0;
};



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



class TakeCommand : public Command
{
public:
    void apply(ListWidgetHandler *handler) {
        QSharedPointer<QListWidgetItem> item(handler->takeItem());
        (void)item;
    }
};

} // namespace Detail

void ListWidgetHandler::handleEvent(const TraceEntry &entry)
{
    using namespace Detail;

    if (entry.isBegin) {
        QListWidgetItem *item = makeItem(entry);
        addItem(item);

        Command *command (new TakeCommand());
        undoList.push_back(command);

    } else {
        QListWidgetItem *item = takeItem();

        Command *command (new AddCommand(item));
        undoList.push_back(command);
    }
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

void ListWidgetHandler::reset()
{
    if (!listWidget->count() == 0)
        listWidget->clear();

    if (!undoList.empty())
        undoList.clear();
}

void ListWidgetHandler::addItem(QListWidgetItem *item)
{
    listWidget->addItem(item);
    listWidget->scrollToItem(item);
}

QListWidgetItem *ListWidgetHandler::takeItem()
{
    QListWidgetItem *item = listWidget->takeItem(listWidget->count() - 1);
    return item;
}

QListWidgetItem* ListWidgetHandler::makeItem(const TraceEntry &entry) const
{
    QListWidgetItem *item = new QListWidgetItem(entry.context, listWidget);
    item->setBackground(
        (listWidget->count() % 2)?
            Qt::white :
            Qt::lightGray
        );

    QVariant data;
    data.setValue(entry);

    item->setData(Qt::UserRole, data);

    return item;
}

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
