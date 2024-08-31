#include "type.h"

// Number class definitions
const QMap<const Number::Operations, QString> Number::operations_view =
{
    { Number::Operations::Equal,          "="  },  { Number::Operations::NotEqual,       "<>" },
    { Number::Operations::Greater,        ">"  },  { Number::Operations::Less,           "<"  },
    { Number::Operations::GreaterOrEqual, ">=" },  { Number::Operations::LessOrEqual,    "<=" }
};
const QMap<const Number::Operations, std::function<bool(QVariant l, QVariant r)>> Number::operations_compare =
{
    { Number::Operations::Equal,          [](QVariant l, QVariant r)    { return QVariant::compare(l, r) == 0; } },
    { Number::Operations::NotEqual,       [](QVariant l, QVariant r)    { return QVariant::compare(l, r) != 0; } },
    { Number::Operations::Greater,        [](QVariant l, QVariant r)    { return QVariant::compare(l, r) > 0;  } },
    { Number::Operations::Less,           [](QVariant l, QVariant r)    { return QVariant::compare(l, r) <  0; } },
    { Number::Operations::GreaterOrEqual, [](QVariant l, QVariant r)    { return QVariant::compare(l, r) >= 0; } },
    { Number::Operations::LessOrEqual,    [](QVariant l, QVariant r)    { return QVariant::compare(l, r) <= 0; } }
};

bool Number::isContains(const QString& op) const
{
    return operations_compare.contains(operations_view.key(op));
}

bool Number::compareValue(QVariant left, QVariant right, const QString& operation) const
{
    Operations op = operations_view.key(operation);
    return ( operations_compare.contains(op) ) ? operations_compare[op](left, right) : false;
}

QString Number::getName() const
{
    return "Number";
}

QString Number::getOperationView(Operations operation) const
{
    return ( operations_view.contains(operation) )? operations_view[operation] : "Unknown operation" ;
}

QStringList Number::getOperations() const
{
    return operations_view.values();
}

// String class definitions
const QMap<const String::Operations, QString> String::operations_view =
{
    {String::Operations::StartsWith, "Начинается с"}, {String::Operations::EndsWith,   "Заканчивается на"},
    {String::Operations::Contains,   "Содержит"},
};
const QMap<const String::Operations, std::function<bool(QString l, QString r)>> String::operations_compare =
{
    {String::Operations::StartsWith, [](QString l, QString r){ return l.indexOf(r) == 0; }                   },
    {String::Operations::EndsWith,   [](QString l, QString r){ return l.size() - l.indexOf(r) == r.size(); } },
    {String::Operations::Contains,   [](QString l, QString r){ return l.indexOf(r) != -1; }                  },
};

bool String::isContains(const QString& op) const
{
    return operations_compare.contains(operations_view.key(op));
}

bool String::compareValue(QVariant left, QVariant right, const QString& operation) const
{
    Operations op = operations_view.key(operation);
    if(left.userType() == QMetaType::QString && right.userType() == QMetaType::QString )
        return ( operations_compare.contains(op) ) ? operations_compare[op](left.toString(), right.toString()) : false;
    return false;
}

QString String::getName() const
{
    return "String";
}

QString String::getOperationView(Operations operation) const
{
    return ( operations_view.contains(operation) )? operations_view[operation] : "Unknown operation" ;
}

QStringList String::getOperations() const
{
    return operations_view.values();
}
