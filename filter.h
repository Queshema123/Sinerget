#ifndef FILTER_H
#define FILTER_H

#include <QMap>
#include <functional>
#include <QVariant>
#include <QRegularExpression>
#include "info.h"

namespace Filter
{

    enum class Operation{ Equal, NotEqual, Less, LessOrEqual, Bigger, BiggerOrEqual, StartsWith, Contains, EndsWith };
    enum class Field{ Name, Value, Labels };
    enum class NumberOperations{Equal, NotEqual, Less, LessOrEqual, Bigger, BiggerOrEqual};
    enum class StringOperations{StartsWith, Contains, EndsWith};

    static const  QMap<Filter::Operation, QString> views =
    {
        {Filter::Operation::Equal,  "="},  {Filter::Operation::NotEqual,      "<>"},
        {Filter::Operation::Less,   "<" }, {Filter::Operation::LessOrEqual,   "<="},
        {Filter::Operation::Bigger, ">"},  {Filter::Operation::BiggerOrEqual, ">="},
        {Filter::Operation::StartsWith, "начинается с"}, {Filter::Operation::Contains, "содержит"},
        {Filter::Operation::EndsWith, "заканчивается на"},
    };

    static const  QMap<Filter::Operation, std::function<bool(QVariant l, QVariant r)> > compare =
    {
        {Filter::Operation::Equal,          [](QVariant l, QVariant r) { return QVariant::compare(l, r) == 0; }},
        {Filter::Operation::NotEqual,       [](QVariant l, QVariant r) { return QVariant::compare(l, r) != 0; }},
        {Filter::Operation::Bigger,         [](QVariant l, QVariant r) { return QVariant::compare(l, r) > 0;  }},
        {Filter::Operation::Less,           [](QVariant l, QVariant r) { return QVariant::compare(l, r) < 0;  }},
        {Filter::Operation::BiggerOrEqual,  [](QVariant l, QVariant r) { return QVariant::compare(l, r) >= 0; }},
        {Filter::Operation::LessOrEqual,    [](QVariant l, QVariant r) { return QVariant::compare(l, r) <= 0; }},

        {Filter::Operation::StartsWith,     [](QVariant l, QVariant r) {
             return l.toString().indexOf(r.toString(), 0, Qt::CaseInsensitive) == 0;
        }},
        {Filter::Operation::EndsWith,       [](QVariant l, QVariant r) {
            QString left = l.toString(), right = r.toString();
            return left.size() - left.indexOf(right, 0, Qt::CaseInsensitive) == right.size();
         }},
        {Filter::Operation::Contains,       [](QVariant l, QVariant r) { return l.toString().indexOf(r.toString(), 0, Qt::CaseInsensitive) != -1; }}
    };

    static const  QMap<Filter::Field, QString> field_view =
    {
        {Filter::Field::Value,  "Значение"   }, {Filter::Field::Name, "Название"},
        {Filter::Field::Labels,  "Параметры" },
    };

    static const  QMap<Filter::NumberOperations, QString> number_operation_view =
    {
        {Filter::NumberOperations::Equal,  "="},  {Filter::NumberOperations::NotEqual,      "<>"},
        {Filter::NumberOperations::Less,   "<" }, {Filter::NumberOperations::LessOrEqual,   "<="},
        {Filter::NumberOperations::Bigger, ">"},  {Filter::NumberOperations::BiggerOrEqual, ">="},
    };

    static const  QMap<Filter::StringOperations, QString> string_operation_view =
    {
        {Filter::StringOperations::StartsWith, "начинается с"  }, {Filter::StringOperations::Contains, "содержит"},
        {Filter::StringOperations::EndsWith, "заканчивается на"},
    };

    inline bool isField(const QString& name)  { return field_view.values().contains(name); }
    inline bool isDate(const QString& val)
    {
        static QRegularExpression re("\\d+.*.\\d+:\\d+:\\d+");
        return re.match(val).hasMatch();
    }
    bool compareValues(const QVariant& left, const QVariant& right, const QString &operation);
    bool checkCondition(const QVariant& value, const Info& condition);
    bool checkConditions(const Info& row_value, const QList<Info> &conditions );

    inline QString getView(Operation operation)  { return views[operation]; }
    inline Operation getOperation(const QString& name)  { return views.key(name); }
    inline QString getView(Field field)  { return field_view[field]; }
    inline Field getField(const QString& field)  { return field_view.key(field); }
    QVariant getValue(const QString& value);
    QVariant getValueToCondition(const Info& data, const Info& condition);
    inline QStringList getFieldsView() { return field_view.values(); }
    QStringList getOperations(const QVariant &value) ;
}

#endif // FILTER_H
