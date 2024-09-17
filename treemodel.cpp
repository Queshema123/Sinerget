#include "treemodel.h"

#include <QStringView>

TreeModel::TreeModel( QObject *parent)
    : QAbstractItemModel{ parent }, root_item{ std::make_unique<TreeItem>(QVariantList{ tr("Metric"), tr("Value") } ) }
{}

TreeModel::TreeModel(const QVector<Token>& tokens, QObject* parent)
    : QAbstractItemModel{ parent }, root_item{ std::make_unique<TreeItem>(QVariantList{ tr("Metric"), tr("Value") } ) }
{
    setupModelData(tokens);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto *item = static_cast<const TreeItem*>( index.internalPointer() );
    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    return index.isValid() ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole ?
               root_item->data(section) : QVariant{};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return {};

    TreeItem* parent_item = parent.isValid() ? static_cast<TreeItem*>( parent.internalPointer() ) : root_item.get();

    if(auto *child_item = parent_item->child(row))
        return createIndex(row, column, child_item);

    return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return {};

    auto *child_item = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parent_item = child_item->parentItem();

    return parent_item != root_item.get() ? createIndex(parent_item->row(), 0, parent_item) : QModelIndex{};
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0)
        return 0;

    const TreeItem* parent_item =  parent.isValid() ? static_cast<const TreeItem*>(parent.internalPointer()) : root_item.get();

    return parent_item->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return static_cast<TreeItem*>( parent.internalPointer() )->columntCount();
    return root_item->columntCount();
}

void TreeModel::setupModelData(const QVector<Token> &tokens)
{
    foreach (const Token& token, tokens) {
        auto root = std::make_unique<TreeItem>( QVariantList{token.getMetricName(), token.getMetricValue()} );
        root_item->appendChild(root);
        root = root_item->child(0);
        for(auto [key, value] : token.getLabels().asKeyValueRange())
        {
            root->appendChild( std::make_unique<TreeItem>( QVariantList{key, value}, root ) );
        }
    }
}
