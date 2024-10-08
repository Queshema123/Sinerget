#include "searchwidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

SearchWidget::SearchWidget(QWidget* parent) :
    FilterWidget{parent}, element{0}, section{0}, element_number{1}
{

    addControlWidgets(this->layout());

    connect(this, &SearchWidget::filteredData, this, [this](const QVector<Token> &data){
        is_search = true;
        max = data.size();
        this->findChild<QLabel*>("MaxElementLabel")->setText(QString::number(max));
    });
    QPushButton* btn = this->findChild<QPushButton*>("SubmitButton");
    btn->setText("Найти");
    disconnect(btn, &QPushButton::clicked, this, &SearchWidget::submit);
    connect(btn,    &QPushButton::clicked, this, &SearchWidget::find);
    connect(this,   &SearchWidget::toClear,  this, &SearchWidget::clearData);
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

    main_wgt->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(main_wgt, Qt::AlignLeft);

    connect(prev_btn, &QPushButton::clicked, this, &SearchWidget::previous);
    connect(next_btn, &QPushButton::clicked, this, &SearchWidget::next);
    connect(this, &SearchWidget::searchedElementNumber, current_element_lbl, [current_element_lbl, this](qsizetype i){
        current_element_lbl->setText(QString::number( i % (max+1) ));
    });
}

void SearchWidget::next()
{
    if(!is_search || this->filtered_sections[section].size() == 0 )
        return;

    ++element;
    element_number = ( element_number+1 <= max ) ? element_number+1 : element_number;
    if(element >= filtered_sections[section].size() )
    {
        element = 0;
        element_number = (section == filtered_sections.size()-1) ? 0 : element_number;
        section = (section == filtered_sections.size()-1) ? section : section + 1;
    }

    if( filtered_sections[section].size() > 0 )
    {
        emit searchedElementIndex( data.indexOf( filtered_sections[section][element] ) );
        emit searchedElementNumber( element_number );
    }
}

void SearchWidget::previous()
{
    if(!is_search || this->filtered_sections[section].size() == 0 )
        return;

    --element;
    element_number = ( element_number-1 > 0 ) ? element_number-1 : element_number;
    if(element < 0)
    {
        section = (section < 1) ? section : section - 1;
        element = filtered_sections[section].size() - 1;
    }

    if( element >= 0 )
    {
        emit searchedElementIndex( data.indexOf( filtered_sections[section][element] ) );
        emit searchedElementNumber( element_number );
    }
}

void SearchWidget::find()
{
    this->submit();
    emit this->status("Поиск");
}

void SearchWidget::clearData(bool is_clear)
{
    if(is_clear)
        section = 0;
}
