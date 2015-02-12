#include "traceentrylistfilterproxy.hpp"
#include "templateeventhandlers/traceentrylist.h"

namespace Templar {

EntryListSortFilterProxy::EntryListSortFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent), usedSourceFileModel{nullptr} {}

bool EntryListSortFilterProxy::lessThan(const QModelIndex &left,
                                        const QModelIndex &right) const {
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    switch (left.column()) {
    case 0:
    case 1:
        return leftData.toString() < rightData.toString();
    case 4:
        return leftData.toDouble() < rightData.toDouble();
    default:
        return leftData.toUInt() < rightData.toUInt();
    }
}

bool EntryListSortFilterProxy::filterAcceptsRow(int sourceRow,
                                                const QModelIndex &) const {
    if (usedSourceFileModel) {
        EntryListModelAdapter *psource =
            dynamic_cast<EntryListModelAdapter *>(sourceModel());
        UsedFileMap &usedFiles = usedSourceFileModel->nodeIdMap;
        UsedFileMap::iterator found =
            usedFiles.find(psource->proxy[sourceRow]->instantiation.fileId);
        if (found != usedFiles.end())
            return (*found)->visible;
    }
    return false;
}

void EntryListSortFilterProxy::fileFilterDataChanged(
    const QModelIndex & /*topLeft*/, const QModelIndex & /*bottomRight*/) {
    invalidateFilter();
}

void EntryListSortFilterProxy::setUsedSourceFileModel(UsedSourceFileModel *model) {
  usedSourceFileModel = model;
}

}
