#ifndef LISTWIDGETHANDLER_H
#define LISTWIDGETHANDLER_H

#include "templateeventhandler.h"
#include <vector>

#include <QSharedPointer>

class QListWidget;
class QListWidgetItem;

namespace Templar {

namespace Detail {

    class Command;

} // namespace Detail

class ListWidgetHandler : public TemplateEventHandler
{
public:
    ListWidgetHandler(QListWidget *listW) :
        listWidget(listW) {}

    void handleEvent(const TraceEntry &entry);
    void undoEvent();
    void reset();
    void forward(const std::vector<TraceEntry> & entryVec);
    void rewind(unsigned int);

public:
    void addItem(QListWidgetItem *item);
    QListWidgetItem* takeItem();

private:
    QListWidgetItem* makeItem(const TraceEntry& entry) const;

private:
    QListWidget *listWidget;
    std::vector<Detail::Command*> undoList;
};

} // namespace Templar

#endif // LISTWIDGETHANDLER_H
