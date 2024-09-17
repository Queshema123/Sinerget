#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariantList>
#include <memory>
#include <vector>

#include "token.h"

class TreeItem
{
public:
    explicit TreeItem(QVariantList data, TreeItem *parent = nullptr);

    void appendChild(std::unique_ptr<TreeItem> &&child);
    TreeItem* child(int row);
    int childCount() const;
    int columntCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem* parentItem();
private:
    std::vector<std::unique_ptr<TreeItem>> child_items;
    QVariantList item_data;
    TreeItem* parent_item;
};

#endif // TREEITEM_H
