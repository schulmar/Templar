#include "traceentry.h"
#include "usedsourcefilemodel.h"
#include <QBrush>
#include <QColor>
namespace Templar
{

TraceEntry::iterator::iterator(const TraceEntry *first)
    :currentEntry(first)
{

}

TraceEntry const *TraceEntry::iterator::moveToNextSibling(TraceEntry const *entry)
{
    if(entry->parent!=nullptr)
    {
        QVector<TraceEntry>::iterator position = std::find_if(entry->parent->children.begin(),entry->parent->children.end(),
                                                              [&](TraceEntry &entry){ return &entry == currentEntry;});
        position++;
        if(position == entry->parent->children.end())
            return moveToNextSibling(entry->parent);
        else
            return &(*position);
    }
    return nullptr;
}

TraceEntry::iterator& TraceEntry::iterator::operator++()
{
    if(currentEntry==nullptr)
        return *this;
    if(currentEntry->children.empty())
    {
        currentEntry = moveToNextSibling(currentEntry);
    }
    else
        currentEntry = &currentEntry->children.front();
    return *this;
}


EntryListModelAdapter::EntryListModelAdapter(QObject *parent, const TraceEntry &entry)
    : QAbstractListModel(parent), entry(entry)
{
    updateProxyData();
}
void EntryListModelAdapter::updateProxyData()
{
    proxy.clear();
    proxy.resize(entry.children.size());
    for(int i=0;i<entry.children.size();++i)
    {
        proxy.at(i) = &entry.children.at(i);
    }
}

QModelIndex EntryListModelAdapter::index(int row, int column, const QModelIndex &parent) const
{

    if(row < proxy.size())
        return createIndex(row,column,(void*)(proxy.at(row)));
    else
        return QModelIndex();
}

QVariant EntryListModelAdapter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    static Qt::GlobalColor backgrounds[] = {
        Qt::white
        , Qt::gray
        , Qt::green
        , Qt::yellow
        , Qt::blue
        , Qt::cyan
        , Qt::lightGray
        , Qt::darkYellow
        , Qt::darkBlue
        , Qt::darkGreen
    };

    switch(role)
    {
    case Qt::DisplayRole:
    {
        switch(col)
        {
        case 0:
            return entry.children.at(row).context;
        case 1:
            return entry.children.at(row).sourcefile;
        case 2:
            return QString::number(entry.children.at(row).memoryUsage);
        case 3:
            return QString::number(entry.children.at(row).children.size());
        }
    }
        break;
    case Qt::ToolTipRole:
    {
        return entry.children.at(row).context;
    }
    case Qt::BackgroundRole:
        return QBrush(backgrounds[entry.children.at(row).kind]);
    }


    return QVariant();
}

EntryListSortFilterProxy::EntryListSortFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{}

bool EntryListSortFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    return leftData.toUInt() < rightData.toUInt();
}
bool EntryListSortFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &) const
{
    EntryListModelAdapter *psource = dynamic_cast<EntryListModelAdapter*>(sourceModel());
    UsedFileMap &usedFiles = UsedSourceFileModel::nodeIdMap;
    UsedFileMap::iterator found = usedFiles.find(psource->proxy[sourceRow]->instantiation.fileId);
    if(found != usedFiles.end())
        return (*found)->visible;
    return false;
}

void EntryListSortFilterProxy::fileFilterDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    invalidateFilter();
}

}
