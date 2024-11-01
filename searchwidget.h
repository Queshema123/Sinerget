#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include "filterwidget.h"
#include "customproxymodel.h"

class SearchWidget : public FilterWidget
{
    Q_OBJECT

    CustomProxyModel* finded_item_model;
    int row, max;

    void addControlWidgets(QLayout* main_layout);
public:
    explicit SearchWidget(QWidget* parent = nullptr);
public slots:
    void next();
    void previous();
    void find();
    void clearSwitch();
    void findValues(const QList<QList<Info>>& info);
    void setData(QAbstractItemModel* model);
    void changeInfo(const QList<QList<Info>>& info);
signals:
    void selectIndex(qsizetype idx);
    void changeCurrentElement(int number);
    void setMaxElementNumber(int max);
};

#endif // SEARCHWIDGET_H
