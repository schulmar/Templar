#include "traceentrylist.h"
#include "usedsourcefilemodel.h"
#include <QBrush>
#include <QColor>
#include <helper/breakstring.hpp>

namespace Templar {

const std::vector<EntryListModelAdapter::Header>
    EntryListModelAdapter::headers = {
        {"Context", "Context of the event"},
        {"File", "File this event is taking place in"},
        {"Memory", "Total memory consumption of clang during this event"},
        {"#Children", "The number of child events"},
        {"Duration", "The duration of this event in microseconds"}};

EntryListModelAdapter::EntryListModelAdapter(
    QObject *parent, const TraceEntry &entry,
    UsedSourceFileModel *usedSourceFileModel)
    : QAbstractListModel(parent), entry(entry),
      usedSourceFileModel{usedSourceFileModel} {
    updateProxyData();
}
void EntryListModelAdapter::updateProxyData()
{
    proxy.clear();
    proxy.resize(entry.children.size());
    for(int i=0;i<entry.children.size();++i)
    {
        proxy.at(i) = entry.children.at(i);
    }
}

QModelIndex EntryListModelAdapter::index(int row, int column,
                                         const QModelIndex & /*parent*/) const {

    if(row < static_cast<int>(proxy.size()))
        return createIndex(row,column,static_cast<void*>(proxy.at(row).data()));
    else
        return QModelIndex();
}

QVariant EntryListModelAdapter::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const {
    try {
        switch (role) {
        case Qt::DisplayRole:
            return headers.at(section).caption;
            break;
        case Qt::ToolTipRole:
            return headers.at(section).tooltip;
            break;
        default:
            return QAbstractListModel::headerData(section, orientation, role);
        }
    } catch (std::out_of_range const&) {
        return QVariant();
    }
}

int EntryListModelAdapter::columnCount(const QModelIndex &) const {
    return headers.size();
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

    auto const& element = entry.children.at(row);

    switch(role)
    {
    case Qt::DisplayRole:
    {
        switch(col)
        {
        case 0:
            return element->context;
        case 1: {
            auto found = usedSourceFileModel->nodeIdMap.find(element->sourceFileId);
            if(found != usedSourceFileModel->nodeIdMap.end()) {
              return (*found)->name;
            }
        }
        case 2:
            return QString::number(element->memoryUsage);
        case 3:
            return QString::number(element->children.size());
        case 4:
            return QString::number(
                std::chrono::duration_cast<std::chrono::microseconds>(
                    element->getDuration())
                    .count());
        }
    }
        break;
    case Qt::ToolTipRole:
    {
        const char *instantiationKindName = "invalid kind index";
        try {
            instantiationKindName =
                TraceEntry::InstantiationKindNames.at(element->kind);
        } catch (std::out_of_range const&) {}
        return QString("%1: %0")
            .arg(Helper::breakString(element->context))
            .arg(instantiationKindName);
    }
    case Qt::BackgroundRole:
        return QBrush(backgrounds[element->kind]);
    }


    return QVariant();
}

}  // namespace Templar

