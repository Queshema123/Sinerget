#include "customproxymodel.h"
#include "filter.h"

CustomProxyModel::CustomProxyModel(QObject* parent)
    : QSortFilterProxyModel{parent}
{
    this->setDynamicSortFilter(false);
}

bool CustomProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(conditions.size() == 0 )
        return true;

    QVariant left  = sourceModel()->index(source_row, 0, source_parent).data();
    QVariant right = sourceModel()->index(source_row, 1, source_parent).data();
    Info row_data(left.toString(), "", right.toString());

    return Filter::checkConditions(row_data, conditions);
}
