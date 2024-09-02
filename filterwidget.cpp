#include "filterwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QRegularExpression>

const QMap<FilterWidget::Field, Type*> FilterWidget::type_to_field =
{
    { FilterWidget::Field::Metric, new String }, { FilterWidget::Field::Value, new Number },
    { FilterWidget::Field::Labels, new String }
};
const QMap<FilterWidget::Field, QString> FilterWidget::field_views =
{
    { FilterWidget::Field::Metric, "Метрика" }, { FilterWidget::Field::Value, "Значение" },
    { FilterWidget::Field::Labels, "Аттрибуты" }
};

void FilterWidget::addBtns(QVBoxLayout* main_layout)
{
    QWidget* btns_wgt = new QWidget;
    QHBoxLayout* btns_layout = new QHBoxLayout(btns_wgt);
    main_layout->addWidget(btns_wgt, 0, Qt::AlignTop);

    QPushButton* submit_btn = new QPushButton("Submit");
    QPushButton* add_btn = new QPushButton("Add");
    QPushButton* delete_btn = new QPushButton("Delete");
    QPushButton* clear_btn = new QPushButton("Clear");

    submit_btn->setObjectName("SubmitButton");
    add_btn->setObjectName("AddButton");
    delete_btn->setObjectName("DeleteButton");
    clear_btn->setObjectName("ClearButton");

    connect(submit_btn, &QPushButton::clicked, this, &FilterWidget::submit);
    connect(add_btn,    &QPushButton::clicked, this, &FilterWidget::add);
    connect(delete_btn, &QPushButton::clicked, this, &FilterWidget::deleteDataFilter);
    connect(clear_btn,  &QPushButton::clicked, this, &FilterWidget::clear);

    btns_layout->addWidget(submit_btn);
    btns_layout->addWidget(add_btn);
    btns_layout->addWidget(delete_btn);
    btns_layout->addWidget(clear_btn);
}

FilterWidget::FilterWidget(QWidget* parent) : QDialog{parent}
{
    setModal(true);
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    addBtns(main_layout);
}

QString FilterWidget::getFieldView(Field field)
{
    return ( field_views.contains(field) ) ? field_views[field] : "Неизвестное поле";
}

FilterWidget::Field FilterWidget::getFieldByName(const QString& name)
{
    return field_views.key(name);
}

QStringList FilterWidget::getOperationsTo(const QString& value)
{
    QRegularExpression re("^[0-9]*$"); // Проверка на число
    if( re.match(value).hasMatch() )
        return type_to_field[Field::Value]->getOperations(); // Field::Value - число
    else
        return type_to_field[Field::Metric]->getOperations(); // Field::Metric - строка
    return {};
}

void FilterWidget::fillFieldsBox(QComboBox* box)
{
    foreach (const QString& name, labels.keys())
        box->addItem(name);

    foreach (const QString& name, field_views.values() )
        box->addItem(name);
}

void FilterWidget::fillOperationsBox(const QString& field)
{
    QComboBox* box = qobject_cast<QComboBox*>( sender() )->parentWidget()->findChild<QComboBox*>("Operation");
    box->clear();

    QStringList operations;
    if( getFieldByName(field) == Field::Labels )
        operations = getOperationsTo(field);
    else
        operations = type_to_field.value( getFieldByName(field) )->getOperations();

    foreach (const QString& operation, operations) {
        box->addItem(operation);
    }
}

void FilterWidget::add()
{
    QVBoxLayout* main_layout = qobject_cast<QVBoxLayout*>( this->layout() );
    QWidget* filter_line_wgt = new QWidget;
    filter_line_wgt->setObjectName("FilterLine");
    main_layout->addWidget(filter_line_wgt);

    QHBoxLayout* filter_line_layout = new QHBoxLayout(filter_line_wgt);
    QComboBox* field_box = new QComboBox;
    QComboBox* operation_box = new QComboBox;
    QLineEdit* input_value_wgt = new QLineEdit;

    field_box->setObjectName("Field");
    operation_box->setObjectName("Operation");
    input_value_wgt->setObjectName("InputValue");

    filter_line_layout->addWidget(field_box);
    filter_line_layout->addWidget(operation_box);
    filter_line_layout->addWidget(input_value_wgt);

    fillFieldsBox(field_box);

    connect(field_box, &QComboBox::currentTextChanged, this, &FilterWidget::fillOperationsBox);
}

void FilterWidget::deleteDataFilter()
{
    auto wgts = this->findChildren<QWidget*>("FilterLine");
    if( !wgts.size() )
        return;

    QWidget* deleted_wgt = wgts.last();
    this->layout()->removeWidget(deleted_wgt);
    deleted_wgt->deleteLater();
}

void FilterWidget::clear()
{
    QList<QWidget*> line_wgts = this->findChildren<QWidget*>("FilterLine");
    foreach(QWidget* wgt, line_wgts)
    {
        wgt->findChild<QComboBox*>("Field")->setCurrentIndex(0);
        wgt->findChild<QComboBox*>("Operation")->setCurrentIndex(0);
        wgt->findChild<QLineEdit*>("InputValue")->clear();
    }
    emit filteredData( {} );
}

QVector<Info> FilterWidget::parseFilterInfo()
{
    QVector<Info> info;

    QList<QWidget*> line_wgts = this->findChildren<QWidget*>("FilterLine");
    foreach(QWidget* wgt, line_wgts)
    {
        Info inf;
        inf.field = wgt->findChild<QComboBox*>("Field")->currentText();
        inf.operation = wgt->findChild<QComboBox*>("Operation")->currentText();
        inf.value = wgt->findChild<QLineEdit*>("InputValue")->text();
        info.push_back( std::move(inf) );
    }

    return info;
}

bool FilterWidget::matchToken(const Info& info, const Token& token)
{
    Field f = field_views.key(info.field);
    QStringList vals;
    switch (f) {
    case Field::Labels:
        vals.append( token.getLabels().keys().join("") );
        break;
    case Field::Metric:
        vals.append( { token.getMetricName() } );
        break;
    case Field::Value:
        vals.append( token.getMetricValue() );
        break;
    default:
        break;
    }

    const Type* field_type = type_to_field[f];
    foreach (const QString& str, vals) {
        if( !field_type->compareValue( str, info.value, info.operation ) )
            return false;
    }

    return true;

}

QVector<Token> FilterWidget::applyFilterToData(const QVector<Info>& filter_info)
{
    QVector<Token> filtered_tokens;

    foreach (const Token& token, data) {
        bool isMatch{true};
        foreach (const Info& inf, filter_info) {
            if( !matchToken( inf, token ) )
            {
                isMatch = false;
                break;
            }
        }

        if(isMatch)
            filtered_tokens.push_back(token);
    }

    return filtered_tokens;
}

void FilterWidget::submit()
{
    filtered_data = applyFilterToData(parseFilterInfo());
    emit filteredData(filtered_data);
}

void FilterWidget::setData(const QVector<Token>& tokens)
{
    data = tokens;
    for(const auto& token : tokens)
        for(auto [key, value] : token.getLabels().asKeyValueRange() )
            labels.insert(key, value);
}

