#include "searchwidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

SearchWidget::SearchWidget(QWidget* parent) :
    FilterWidget{parent}, finded_item_model{nullptr}, row{0}, max{1}
{
    finded_item_model = new CustomProxyModel(this);
    addControlWidgets(this->layout());
    QPushButton* btn = this->findChild<QPushButton*>("SubmitButton");
    disconnect(btn, &QPushButton::clicked,   this, &FilterWidget::submit);
    connect(btn,    &QPushButton::clicked,   this, &SearchWidget::find);
    connect(this,   &FilterWidget::info,     this, &SearchWidget::findValues);
    connect(this,   &FilterWidget::isClear,  this, &SearchWidget::clearSwitch);
}

void SearchWidget::addControlWidgets(QLayout* main_layout)
{
    QWidget* main_wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setObjectName("SearchedWidgetsLayout");
    layout->setAlignment(Qt::AlignCenter);

    main_wgt->setLayout(layout);
    main_layout->addWidget(main_wgt);

    QPushButton *prev_btn = new QPushButton("пред.");
    prev_btn->setObjectName("PreviousButton");
    prev_btn->setMaximumSize(50, 30);

    QPushButton *next_btn = new QPushButton("след.");
    next_btn->setObjectName("NextButton");
    next_btn->setMaximumSize(50, 30);

    QLabel *curr_elem_lbl = new QLabel("1");
    curr_elem_lbl->setObjectName("CurrentElementLabel");
    QLabel *max_element_lbl = new QLabel("1");
    max_element_lbl->setObjectName("MaxElementLabel");
    QLabel *separate_lbl = new QLabel("/");

    layout->addWidget(prev_btn);
    layout->addWidget(curr_elem_lbl);
    layout->addWidget(separate_lbl);
    layout->addWidget(max_element_lbl);
    layout->addWidget(next_btn);

    main_wgt->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(main_wgt, Qt::AlignLeft);

    connect(prev_btn, &QPushButton::clicked, this, &SearchWidget::previous);
    connect(next_btn, &QPushButton::clicked, this, &SearchWidget::next);
    connect(this, &SearchWidget::changeCurrentElement, curr_elem_lbl, [curr_elem_lbl, this](int i){ curr_elem_lbl->setText( QString::number( i ) ); });
    connect(this, &SearchWidget::setMaxElementNumber, this, [max_element_lbl, this](int m)
    {
        max = (m > 0) ? m : 1;
        max_element_lbl->setText( QString::number(max) );
    } );
}

void SearchWidget::next()
{
    row = (row+1 < max) ? row+1 : row;
    int model_row{ finded_item_model->getRowIndex(row) };
    emit selectIndex( model_row );
    emit changeCurrentElement(row+1);
}

void SearchWidget::previous()
{
    row = (row-1 >= 0) ? row-1 : row;
    int model_row{ finded_item_model->getRowIndex(row) };
    emit selectIndex( model_row );
    emit changeCurrentElement(row+1);
}

void SearchWidget::setData(QAbstractItemModel* model)
{
    FilterWidget::setData(model);
    finded_item_model->setSourceModel(model);
    emit setMaxElementNumber(finded_item_model->getRowCount());
}

void SearchWidget::findValues(const QList<QList<Info>>& info)
{
    finded_item_model->setFilterConditions(info);
    emit status("Найдено - " + QString::number(finded_item_model->getRowCount()));
    emit setMaxElementNumber(finded_item_model->getRowCount() );
}

void SearchWidget::find()
{
    this->submit();
    emit this->status("Поиск");
}

void SearchWidget::changeInfo(const QList<QList<Info>>& info)
{
    finded_item_model->setFilterConditions(info);
}

void SearchWidget::clearSwitch()
{
    emit setMaxElementNumber(1);
    emit changeCurrentElement(1);
}
