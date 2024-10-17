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
    {
        rows_indexes.push_back(source_row);
        return true;
    }

    QVariant left  = sourceModel()->index(source_row, 0, source_parent).data();
    QVariant right = sourceModel()->index(source_row, 1, source_parent).data();
    Info row_data(left.toString(), "", right.toString());
    foreach (const QList<Info>& inf, conditions)
    {
        if( Filter::checkConditions(row_data, inf) )
        {
            rows_indexes.push_back(source_row);
            return true;
        }
    }

    return false;
}

void CustomProxyModel::setFilterConditions(const QList<QList<Info>>& info)
{
    this->conditions = info;
    this->rows_indexes.clear();
    invalidateFilter();
}
