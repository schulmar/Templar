#ifndef TRACEENTRYLIST_H_
#define TRACEENTRYLIST_H_

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
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
    virtual int rowCount(const QModelIndex &) const { return entry.children.size();}
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
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
class EntryListSortFilterProxy : public QSortFilterProxyModel
{
  Q_OBJECT
public:
    EntryListSortFilterProxy(QObject *parent = 0);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &) const;
     bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public slots:
    void fileFilterDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void setUsedSourceFileModel(UsedSourceFileModel*);
private:
    UsedSourceFileModel *usedSourceFileModel;

};

}  // namespace Templar



#endif /* TRACEENTRYLIST_H_ */
