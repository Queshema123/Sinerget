#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include "filterwidget.h"

class SearchWidget : public FilterWidget
{
    Q_OBJECT

    bool is_search;
    int element, section;
    int max;

    void addControlWidgets(QLayout* main_layout);
public:
    explicit SearchWidget(QWidget* parent = nullptr);
public slots:
    void next();
    void previous();
signals:
    void searchedElementIndex(qsizetype index);
};

#endif // SEARCHWIDGET_H
