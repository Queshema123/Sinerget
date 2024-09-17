#include "treeitem.h"

#include <algorithm>

TreeItem::TreeItem(QVariantList data, TreeItem *parent)
    : item_data{ std::move(data) }, parent_item{ parent }
{}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child)
{
    child_items.push_back(std::move(child));
}

TreeItem* TreeItem::child(int row)
{
    return row >= 0 && row < childCount() ? child_items.at(row).get() : nullptr;
}

int TreeItem::childCount() const
{
    return static_cast<int>( child_items.size() );
}

int TreeItem::columntCount() const
{
    return static_cast<int>( item_data.count() );
}

QVariant TreeItem::data(int column) const
{
    return item_data.value(column);
}

int TreeItem::row() const
{
    if(parent_item == nullptr)
        return 0;
    const auto it = std::find_if(parent_item->child_items.cbegin(), parent_item->child_items.cend(),
        [this](const std::unique_ptr<TreeItem>& tree_item)
        {
            return tree_item.get() == this;
        });

    if(it != parent_item->child_items.cend())
        return std::distance(parent_item->child_items.cbegin(), it);
    Q_ASSERT(false);
    return -1;
}

TreeItem* TreeItem::parentItem()
{
    return parent_item;
}
