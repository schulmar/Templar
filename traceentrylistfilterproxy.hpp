#ifndef TRACEENTRYLISTFILTERPROXY_HPP_
#define TRACEENTRYLISTFILTERPROXY_HPP_

#include <QSortFilterProxyModel>
#include "usedsourcefilemodel.h"

namespace Templar {

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

}

#endif /* TRACEENTRYLISTFILTERPROXY_HPP_ */
