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
    connect(templates_box, &QComboBox::currentTextChanged, this, &FilterWidget::applyFilterTemplate);
}

FilterWidget::FilterWidget(QWidget *parent)
    : QDialog{parent}, addLine{false}
{
    setModal(true);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    addBtns(main_layout);
    this->setObjectName("FilterWidget");
    connect(this, &FilterWidget::rejected, this, &FilterWidget::saveSettins);
    setupSettings();
}

QList<QList<Info>> FilterWidget::getAllInfo() const
{
    QList<QList<Info>> conditions;

    foreach (QWidget* wgt, info_wgts)
    {
        InfoBox* box = qobject_cast<InfoBox*>(wgt);
        if(box)
            conditions.append( box->getInfo() );
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

void FilterWidget::addTemplate(const QString& name)
{
    this->findChild<QComboBox*>("FilterTemplatesBox")->addItem(name);
    emit templateName( name );
}

void FilterWidget::saveFilterTemplate()
{
    QString name{ QInputDialog::getText(this, "Имя шаблона", "Введите имя шаблона") };
    filter_templates.insert(name,  getAllInfo() );
    addTemplate(name);
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
    emit isClear();
}

void FilterWidget::applyFilterTemplate(const QString& name)
{
    clearLayout();
    if(name == "Пусто")
        return;

    for(int i{0}; i < filter_templates[name].size(); ++i )
    {
        InfoBox* box = new InfoBox( this, filter_templates[name][i] );
        info_wgts.append(box);
        this->layout()->addWidget(box);

        if(i != filter_templates[name].size()-1)
            this->addOperationAND();
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

void FilterWidget::saveSettins()
{
    QSettings wgt_settings(this->objectName()+"_settings.ini", QSettings::IniFormat);
    wgt_settings.beginGroup(this->objectName());

    foreach(const QString& key, filter_templates.keys())
    {
        wgt_settings.beginGroup(key);
        for(int i{0}; i < filter_templates[key].size(); ++i)
        {
            wgt_settings.beginGroup(QString::number(i));
            int j{0};
            foreach(const Info& inf, filter_templates[key][i])
            {
                wgt_settings.beginGroup(QString::number(j));
                wgt_settings.setValue( "field", inf.field );
                wgt_settings.setValue( "operation", inf.operation);
                wgt_settings.setValue( "value", inf.value);
                wgt_settings.endGroup();
                ++j;
            }
            wgt_settings.endGroup();
        }
        wgt_settings.endGroup();
    }
    wgt_settings.endGroup();
    wgt_settings.sync();
}

void FilterWidget::parseSettingsFile(QSettings& wgt_settings)
{
    foreach (const QString& name, wgt_settings.childGroups())
    {
        wgt_settings.beginGroup(name);
        QList<QList<Info>> name_info;
        int j{0};
        foreach (const QString& row, wgt_settings.childGroups())
        {
            wgt_settings.beginGroup(row);
            QList<Info> row_info;
            foreach (const QString& col, wgt_settings.childGroups())
            {
                wgt_settings.beginGroup(col);
                row_info.append( Info(wgt_settings.value("field").toString(), wgt_settings.value("operation").toString(), wgt_settings.value("value").toString()) );
                wgt_settings.endGroup();
            }
            wgt_settings.endGroup();
            name_info.append(std::move(row_info));
        }
        filter_templates[name].append(std::move(name_info));
        wgt_settings.endGroup();
        addTemplate(name);
    }
}

void FilterWidget::setupSettings()
{
    QSettings wgt_settings(this->objectName()+"_settings.ini", QSettings::IniFormat);
    wgt_settings.beginGroup(this->objectName());
    parseSettingsFile(wgt_settings);
    wgt_settings.endGroup();
}

FilterWidget::~FilterWidget() { }
