#include "searchwidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

SearchWidget::SearchWidget(QWidget* parent) :
    FilterWidget{parent}, element{0}, section{0}
{

    addControlWidgets(this->layout());

    connect(this, &SearchWidget::filteredData, this, [this](const QVector<Token> &data){
        is_search = true;
        max = data.size();
        this->findChild<QLabel*>("MaxElementLabel")->setText(QString::number(max));
    });
}

void SearchWidget::addControlWidgets(QLayout* main_layout)
{
    QWidget* main_wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setObjectName("SearchedWidgetsLayout");
    layout->setDirection(QBoxLayout::LeftToRight);
    main_wgt->setLayout(layout);
    main_layout->addWidget(main_wgt);

    QPushButton *prev_btn = new QPushButton("prev");
    prev_btn->setObjectName("PreviousButton");
    prev_btn->setMaximumSize(50, 50);

    QPushButton *next_btn = new QPushButton("next");
    next_btn->setObjectName("NextButton");
    next_btn->setMaximumSize(50, 50);

    QLabel *current_element_lbl = new QLabel("1");
    current_element_lbl->setObjectName("CurrentElementLabel");
    QLabel *max_element_lbl = new QLabel("1");
    max_element_lbl->setObjectName("MaxElementLabel");
    QLabel *separate_lbl = new QLabel("/");

    layout->addWidget(prev_btn);
    layout->addWidget(current_element_lbl);
    layout->addWidget(separate_lbl);
    layout->addWidget(max_element_lbl);
    layout->addWidget(next_btn);

    connect(prev_btn, &QPushButton::clicked, this, &SearchWidget::previous);
    connect(next_btn, &QPushButton::clicked, this, &SearchWidget::next);
    connect(this, &SearchWidget::searchedElementIndex, current_element_lbl, [current_element_lbl](qsizetype i){
        current_element_lbl->setText(QString::number(i+1));
    });
}

void SearchWidget::next()
{
    if(!is_search || this->filtered_sections[section].size() == 0 )
        return;

    if(element >= filtered_sections[section].size() )
    {
        element = 0;
        section = (section == filtered_sections.size()-1) ? section : section + 1;
    }

    if( filtered_sections[section].size() > 0 )
    {
        element = (element+1 < filtered_sections[section].size()) ? element+1 : element;
        emit searchedElementIndex( data.indexOf( filtered_sections[section][element] ) );
    }
}

void SearchWidget::previous()
{
    if(!is_search || this->filtered_sections[section].size() == 0 )
        return;

    if(element < 0)
    {
        section = (section < 0) ? section : section - 1;
        element = filtered_sections[section].size() - 1;
    }

    if( element >= 0 )
    {
        element = (element-1 >= 0) ? element-1 : element;
        emit searchedElementIndex( data.indexOf( filtered_sections[section][element] ) );
    }
}
