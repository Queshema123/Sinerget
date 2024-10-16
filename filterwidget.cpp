#include "filterwidget.h"
#include "filter.h"
#include "infobox.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QInputDialog>

QPushButton* FilterWidget::addBtn(const QString& view, const QString& obj_name, QLayout* layout)
{
    QPushButton* btn = new QPushButton(view);
    btn->setObjectName(obj_name);
    layout->addWidget(btn);
    return btn;
}

void FilterWidget::addBtns(QVBoxLayout *main_layout)
{
    QWidget *btns_wgt = new QWidget;
    QVBoxLayout *btns_layout = new QVBoxLayout(btns_wgt);
    QHBoxLayout *f_btns_line = new QHBoxLayout;
    QHBoxLayout *s_btns_line = new QHBoxLayout;
    btns_layout->addLayout(f_btns_line);
    btns_layout->addLayout(s_btns_line);
    main_layout->addWidget(btns_wgt, 0, Qt::AlignTop);

    QPushButton *submit_btn = addBtn("Применить",   "SubmitButton", f_btns_line);
    QPushButton *add_btn    = addBtn("Добавить",    "AddButton",    f_btns_line);
    QPushButton *delete_btn = addBtn("Удалить",     "DeleteButton", f_btns_line);
    QPushButton *clear_btn  = addBtn("Очистить",    "ClearButton",  f_btns_line);
    QPushButton *and_btn    = addBtn("И",           "ANDButton",    s_btns_line);
    QPushButton *save_btn   = addBtn("Сохранить",   "SaveButton",   s_btns_line);

    QComboBox *templates_box = new QComboBox;
    templates_box->setObjectName("FilterTemplatesBox");
    templates_box->addItem("Пусто");
    s_btns_line->addWidget(templates_box);

    connect(submit_btn, &QPushButton::clicked, this, &FilterWidget::submit);
    connect(add_btn,    &QPushButton::clicked, this, &FilterWidget::add);
    connect(delete_btn, &QPushButton::clicked, this, &FilterWidget::deleteLastLine);
    connect(clear_btn,  &QPushButton::clicked, this, &FilterWidget::clearLayout);
    connect(and_btn,    &QPushButton::clicked, this, &FilterWidget::addOperationAND);
    connect(save_btn,   &QPushButton::clicked, this, &FilterWidget::saveFilterTemplate);
}

FilterWidget::FilterWidget(QWidget *parent)
    : QDialog{parent}, addLine{false}
{
    setModal(true);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    addBtns(main_layout);
}

QList<Info> FilterWidget::getAllInfo() const
{
    QList<Info> conditions;

    foreach (QWidget* wgt, info_wgts)
    {
        InfoBox* box = qobject_cast<InfoBox*>(wgt);
        if(box)
            conditions.append( box->getInfo() );
        else
            conditions.append( Info("", "AND", "") );
    }

    return conditions;
}

void FilterWidget::setData(QAbstractItemModel* model)
{
    if(!model)
        return;

    for(int row = 0; row < model->rowCount(); ++row)
    {
        QModelIndex left{ model->index(row, 0) };

        if( !model->hasChildren(left) )
            continue;

        for(int i = 0; i < model->rowCount(left); ++i)
        {
            QModelIndex l{ model->index(i, 0, left) };
            QModelIndex r{ model->index(i, 1, left) };
            if(l.data().toString().size() > 0 )
                labels_and_value[ l.data().toString() ] = r.data();
        }

    }
}

QStringList FilterWidget::getOperations(const QString& field)
{
    if(labels_and_value.contains(field))
       return Filter::getOperations( labels_and_value.value(field));

    return Filter::getOperations(field);
}

void FilterWidget::add()
{
    if(addLine)
    {
        InfoBox* box = qobject_cast<InfoBox*>(info_wgts.last());
        box->addFilterLine();
        return;
    }

    InfoBox* box = new InfoBox;
    box->setObjectName("InfoBox");
    box->setFields( Filter::getFieldsView() + labels_and_value.keys() );
    connect(box, &InfoBox::chooseField, box, [this, box](const QString& field) { box->setOperations( this->getOperations(field) ); });
    box->addFilterLine();
    info_wgts.append(box);
    this->layout()->addWidget(box);
    addLine = true;
}

void FilterWidget::submit()
{
    emit info(getAllInfo());
    emit status("Применен фильтр");
}

void FilterWidget::addOperationAND()
{
    QLabel* lbl = new QLabel("И");
    lbl->setObjectName("AndLabel");
    this->layout()->addWidget( lbl );
    info_wgts.append(lbl);
    addLine = false;
}

void FilterWidget::saveFilterTemplate()
{
    QString name{ QInputDialog::getText(this, "Имя шаблона", "Введите имя шаблона") };
    filter_templates.insert(name,  getAllInfo() );

    emit templateName( name );
}

void FilterWidget::clearLayout()
{
    foreach (QWidget* wgt, info_wgts)
    {
        InfoBox* box = qobject_cast<InfoBox*>(wgt);
        if(box)
            box->deleteAllLines();

        wgt->deleteLater();
    }
    info_wgts.clear();
    addLine = false;
    emit status("Очистка");
    emit info( {} );
}

void FilterWidget::applyFilterTemplate(const QString& name)
{
    clearLayout();
    if(name == "Пусто")
        return;

    QList<Info> info{ filter_templates[name] };
    const Info and_op("", "AND", "");
    for(int start = 0, end = 0; start < info.size(); )
    {
        end = info.indexOf(and_op, start);
        InfoBox* box = new InfoBox( this, info.sliced(start, end-start) );
        info_wgts.append(box);
        this->layout()->addWidget(box);
        this->addOperationAND();
        start = end+1;
    }
}

void FilterWidget::deleteLastLine()
{
    if(info_wgts.size() == 0)
        return;

    QWidget* wgt = info_wgts.last();
    InfoBox* box = qobject_cast<InfoBox*>(wgt);
    if(!box)
    {
        this->layout()->removeWidget(wgt);
        wgt->deleteLater();
        addLine = true;
        return;
    }


    box->deleteLastFilterLine();
    if(!box->hasLines())
    {
        box->deleteLater();
        info_wgts.removeLast();
    }
}
