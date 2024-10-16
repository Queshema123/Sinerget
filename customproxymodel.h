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
    void setFilterConditions(const QList<Info> &conditions) { this->conditions = conditions; invalidateFilter(); }
private:
    QList<Info> conditions;
};

#endif // CUSTOMPROXYMODEL_H
