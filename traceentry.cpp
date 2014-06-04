#include "traceentry.h"
#include "usedsourcefilemodel.h"
#include <QBrush>
#include <QColor>
namespace Templar
{

TraceEntry::iterator::iterator(TraceEntry *first)
    :currentEntry(first)
{

}

TraceEntry *TraceEntry::iterator::moveToNextSibling(TraceEntry *entry)
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
    if(currentEntry->children.empty())
    {
        currentEntry = moveToNextSibling(currentEntry);
    }
    else
        currentEntry = &currentEntry->children.front();
}


EntryListModelAdapter::EntryListModelAdapter(QObject *parent, const TraceEntry &entry)
    : QAbstractListModel(parent), entry(entry)
    {
        updateProxyData();
    }
void EntryListModelAdapter::updateProxyData()
{
    UsedFileMap &usedFiles = UsedSourceFileModel::nodeIdMap;
    proxy.clear();
    proxy.reserve(entry.children.size());
    for(int i=0;i<entry.children.size();++i)
    {
        const TraceEntry *current = &entry.children.at(i);
        UsedFileMap::iterator found = usedFiles.find(current->instantiation.fileId);
        if(found != usedFiles.end())
        {
            if((*found)->visible)
                proxy.push_back(&entry.children.at(i));
        }
    }
    dataChanged(index(0,0),index());
}
/*
void EntryListModelAdapter::sort(int column, Qt::SortOrder order)
{
    std::sort(
                proxy.begin()
                ,proxy.end()
                ,[](const TraceEntry *a, const TraceEntry *b) { return a->memoryUsage < b->memoryUsage;}
    );
}
*/
void EntryListModelAdapter::fileFilterDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    updateProxyData();
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

}
