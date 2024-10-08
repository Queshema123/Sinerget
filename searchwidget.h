#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include "filterwidget.h"

class SearchWidget : public FilterWidget
{
    Q_OBJECT

    bool is_search;
    int element, section, element_number;
    int max;

    void addControlWidgets(QLayout* main_layout);
public:
    explicit SearchWidget(QWidget* parent = nullptr);
public slots:
    void next();
    void previous();
    void find();
private slots:
    void clearData(bool is_clear);
signals:
    void searchedElementIndex(qsizetype index);
    void searchedElementNumber(qsizetype number);
};

#endif // SEARCHWIDGET_H
