#ifndef CUSTOMPROXYMODEL_H
#define CUSTOMPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QList>
#include "info.h"

class CustomProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
public:
    CustomProxyModel(QObject* parent = nullptr);
    void setFilterConditions(const QList<QList<Info>>& info);
    int getRowIndex(int row) const { return (row < rows_indexes.size()) ? rows_indexes[row] : 0; }
    int getRowCount() const { return rows_indexes.size(); }
private:
    QList<QList<Info>> conditions;
    mutable QList<int> rows_indexes;
};

#endif // CUSTOMPROXYMODEL_H
