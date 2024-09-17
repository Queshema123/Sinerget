#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "treeitem.h"
#include "token.h"

#include <QAbstractItemModel>
#include <QVector>
#include <memory>

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(TreeModel)

    explicit TreeModel(QObject* parent = nullptr);
    explicit TreeModel(const QVector<Token>& tokens = {}, QObject* parent = nullptr);
    ~TreeModel() = default;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    void setupModelData(const QVector<Token> &tokens);
private:
    std::unique_ptr<TreeItem> root_item;
};

#endif // TREEMODEL_H
