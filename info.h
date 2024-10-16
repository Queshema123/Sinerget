#ifndef INFO_H
#define INFO_H

#include <QString>

struct Info
{
    QString field, operation, value;
    explicit Info() = default;
    Info(const Info &other)
        : field{other.field}
        , operation{other.operation}
        , value{other.value}
    {}
    Info(const QString &field, const QString &operation, const QString &value)
        : field{field}
        , operation{operation}
        , value{value}
    {}
    Info(Info &&other)
        : field{other.field}
        , operation{other.operation}
        , value{other.value}
    {}
    Info &operator=(const Info &other)
    {
        field = other.field;
        operation = other.operation;
        value = other.value;
        return *this;
    }
    bool operator==(const Info &other) const
    {
        return field == other.field && operation == other.operation && value == other.value;
    }
};

#endif // INFO_H
