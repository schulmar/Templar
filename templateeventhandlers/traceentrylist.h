#ifndef TRACEENTRYLIST_H_
#define TRACEENTRYLIST_H_

#include <QAbstractListModel>
#include "traceentry.h"
#include "usedsourcefilemodel.h"

namespace Templar {

class EntryListModelAdapter : public QAbstractListModel
{
    Q_OBJECT
  public:
    EntryListModelAdapter(QObject *parent, const TraceEntry &entry,
                          UsedSourceFileModel *usedSourceFileModel);
    int columnCount(const QModelIndex &) const override;
    int rowCount(const QModelIndex &) const override { return entry.children.size();}
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    void updateProxyData();
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    const TraceEntry &entry;
    std::vector<traceEntryPtr> proxy;
private:
    struct Header {
        const char *caption;
        const char *tooltip;
    };
    static const std::vector<Header> headers;
    UsedSourceFileModel *usedSourceFileModel;
};

}  // namespace Templar



#endif /* TRACEENTRYLIST_H_ */
