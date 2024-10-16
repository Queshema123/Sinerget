#include "filter.h"

QStringList Filter::getOperations(const QVariant &value)
{
    QVariant val{ value };

    if(value.toString() == getView(Filter::Field::Value))
        return number_operation_view.values();

    if( Filter::isDate(value.toString()) )
        return {};
    else if( val.convert(QMetaType(QMetaType::Double)) )
        return number_operation_view.values();
    else
        return string_operation_view.values();
}

QVariant Filter::getValue(const QString& value)
{
    QVariant val{ value };
    if( val.convert(QMetaType(QMetaType::QDate)) )
        return val;
    else if( val.convert(QMetaType(QMetaType::Double)) )
        return val;

    return value;
}

QVariant Filter::getValueToCondition(const Info& data, const Info& condition)
{ // В зависимости от того на какое из полей накладывается условие(Название, Значение) выбирается из data соотв. значение
    Field field{ Filter::getField(condition.field) };
    QVariant value;
    switch (field) {
    case Field::Name:
    case Field::Labels:
        value = data.field;
        break;
    case Field::Value:
        value = getValue(data.value);
        break;
    default:
        break;
    }
    return value;
}

bool Filter::compareValues(const QVariant& left, const QVariant& right, const QString &operation)
{
    return Filter::compare[getOperation(operation)](left, right);
}

bool Filter::checkCondition(const QVariant& value, const Info& condition)
{
    return Filter::compareValues(value, getValue(condition.value), condition.operation);
}

bool Filter::checkConditions( const Info& row_value, const QList<Info> &conditions )
{
    foreach (const Info& condition, conditions)
        if( !Filter::checkCondition(getValueToCondition(row_value, condition), condition) )
            return false;

    return true;
}
