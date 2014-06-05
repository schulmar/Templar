#ifndef LISTWIDGETHANDLER_H
#define LISTWIDGETHANDLER_H

#include "templateeventhandler.h"
#include <vector>

#include <QSharedPointer>

class QSortFilterProxyModel;
namespace Templar {

namespace Detail {

    class Command;

} // namespace Detail

class ListWidgetHandler : public TemplateEventHandler
{
public:
    ListWidgetHandler(QSortFilterProxyModel *proxyModel) :
        proxyModel(proxyModel) {}

    void handleEvent(const TraceEntry &entry);
    void inspect(const TraceEntry &entry);
    void undoEvent();
    void reset(const TraceEntry &entry);
    void forward(const std::vector<TraceEntry> & entryVec);
    void rewind(unsigned int);

public:
 //   void addItem(QListWidgetItem *item);

   // QListWidgetItem* makeItem(const TraceEntry& entry) const;

private:
    QSortFilterProxyModel *proxyModel;
    std::vector<Detail::Command*> undoList;
};

} // namespace Templar

#endif // LISTWIDGETHANDLER_H
