#include "infobox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>

InfoBox::InfoBox(QWidget* parent, const QList<Info> &info)
    : QWidget{parent}
    , last_clicked_wgt{nullptr}
{
    this->setLayout(new QVBoxLayout() );
    foreach (const Info& inf, info)
    {
        this->setFields( {inf.field} );
        this->setOperations( {inf.operation} );
        this->addFilterLine();
    }
}

void InfoBox::addFilterLine()
{
    QWidget* line_wgt = new QWidget;
    line_wgt->setObjectName("FilterLine");
    QHBoxLayout* line_layout = new QHBoxLayout(line_wgt);

    QComboBox* fields_box     = new QComboBox;
    QComboBox* operations_box = new QComboBox;
    QLineEdit* value          = new QLineEdit;

    fields_box->setObjectName("FieldsBox");
    operations_box->setObjectName("OperationsBox");
    value->setObjectName("InputValue");

    line_layout->addWidget(fields_box);
    line_layout->addWidget(operations_box);
    line_layout->addWidget(value);

    fields_box->addItems( this->fields );
    last_clicked_wgt = line_wgt;

    this->layout()->addWidget(line_wgt);

    QObject::connect( fields_box,     &QComboBox::currentTextChanged, this, &InfoBox::chooseField );
    QObject::connect( operations_box, &QComboBox::currentTextChanged, this, &InfoBox::chooseOperation);
    QObject::connect( value,          &QLineEdit::editingFinished,    this, [this, value](){ emit inputValue(value->text()); } );

    QObject::connect( fields_box,     &QComboBox::textActivated,   this,    [this, line_wgt](){ this->last_clicked_wgt = line_wgt; } );
    QObject::connect( operations_box, &QComboBox::textActivated,   this,    [this, line_wgt](){ this->last_clicked_wgt = line_wgt; } );
    QObject::connect( value,          &QLineEdit::editingFinished, this,    [this, line_wgt](){ this->last_clicked_wgt = line_wgt; } );

    emit chooseField( fields_box->currentText() );
    emit isAddedFilterLine( this->findChildren<QWidget*>("FilterLine").size() < 2 ); // Все виджеты после 1 FilterLine - это подфильтры
}

void InfoBox::deleteLastFilterLine()
{
    QWidgetList lines = this->findChildren<QWidget*>("FilterLine");
    if(lines.isEmpty())
        return;

    QWidget* last_line = lines.last();
    Q_ASSERT_X( last_line, "deleteLastFilterLine in InfoBox", "get nullptr from findChildren list");

    this->layout()->removeWidget(last_line);
    last_line->deleteLater();
    emit isDeletedLastFilterLine( lines.size()-1 == 0 );
}

void InfoBox::setOperations(const QStringList &operations)
{
    QComboBox* box = last_clicked_wgt->findChild<QComboBox*>("OperationsBox");
    Q_ASSERT_X(box, "setOperations in InfoBox", "not find OperationsBox");
    box->clear();
    box->addItems(operations);
}

void InfoBox::setFields(const QStringList &fields)
{
    this->fields = fields;
}

void InfoBox::deleteAllLines()
{
    QWidgetList lines = this->findChildren<QWidget*>("FilterLine");
    foreach (QWidget* line, lines)
    {
        this->layout()->removeWidget(line);
        line->deleteLater();
    }
    emit isClear(true);
}

QList<Info> InfoBox::getInfo() const
{
    QList<Info> info;
    QWidgetList lines = this->findChildren<QWidget*>("FilterLine");
    Info inf;
    foreach (QWidget* line, lines)
    {
        inf.field     = line->findChild<QComboBox*>("FieldsBox")->currentText();
        inf.operation = line->findChild<QComboBox*>("OperationsBox")->currentText();
        inf.value     = line->findChild<QLineEdit*>("InputValue")->text();
        info.push_back( inf );
    }
    return info;
}
